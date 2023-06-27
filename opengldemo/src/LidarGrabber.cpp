
#include "LidarGrabber.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <pcap.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <math.h>


double *CLidarGrabber::cos_lookup_table_ = nullptr;
double *CLidarGrabber::sin_lookup_table_ = nullptr;

CLidarGrabber::CLidarGrabber(std::string pcapFile) : 
                                m_pcapFile(pcapFile) {
    m_readPacketThreadHandle = nullptr;
    lidarGrabberInit();
}

CLidarGrabber::CLidarGrabber(std::string ipAddress, int port) : 
                                m_ipAddress(ipAddress), m_port(port) {
    m_readPacketThreadHandle = nullptr;
    lidarGrabberInit();
}

CLidarGrabber::CLidarGrabber(std::string pcapFile, std::string ipAddress, int port) : 
                                m_pcapFile(pcapFile), m_ipAddress(ipAddress), m_port(port) {
    m_readPacketThreadHandle = nullptr;
    lidarGrabberInit();
}

CLidarGrabber::~CLidarGrabber() {
    if (nullptr != m_readPacketThreadHandle)
    {
        delete m_readPacketThreadHandle;
        m_readPacketThreadHandle = nullptr;
    }
    if (nullptr != m_processPacketThreadHandle)
    {
        m_processPacketThreadHandle->join();
        delete m_processPacketThreadHandle;
        m_processPacketThreadHandle = nullptr;
    }

    (void)close(m_sockfd);
}

int CLidarGrabber::lidarGrabberInit() { 
    int ret = 0;

    last_azimuth_ = 65000;
    if (cos_lookup_table_ == nullptr && sin_lookup_table_ == nullptr)
    {
        cos_lookup_table_ = static_cast<double*>(malloc (HDL_NUM_ROT_ANGLES * sizeof (*cos_lookup_table_)));
        sin_lookup_table_ = static_cast<double*>(malloc (HDL_NUM_ROT_ANGLES * sizeof (*sin_lookup_table_)));
        for (std::uint16_t i = 0; i < HDL_NUM_ROT_ANGLES; i++)
        {
        double rad = (M_PI / 180.0) * (static_cast<double> (i) / 100.0);
        cos_lookup_table_[i] = std::cos (rad);
        sin_lookup_table_[i] = std::sin (rad);
        }
    }

    loadHDL32Corrections();
    sweep_xyzi_signal_ = new boost::signals2::signal<sig_sweep_point_cloud_xyzi>();

    m_readPacketThread = false;
    if (!m_pcapFile.empty()) {
        m_readMode = READMODE::READFROMPCAP;
    } else if (!m_ipAddress.empty()) {
        m_readMode = READMODE::READFROMSOCKET;

        if (m_sockfd != -1) {
            (void)close(m_sockfd);
        }

        // connect to Velodyne UDP port
        printf("INFO--Opening UDP socket: port %hu", uint16_t(m_port));

        m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (m_sockfd == -1) {
            printf("ERROR--Init socket failed, UDP port is %d", m_port);
            return -1;
        }

        sockaddr_in my_addr;                     // my address information
        memset(&my_addr, 0, sizeof(my_addr));    // initialize to zeros
        my_addr.sin_family = AF_INET;            // host byte order
        my_addr.sin_port = htons(uint16_t(m_port));// short, in network byte order
        my_addr.sin_addr.s_addr = INADDR_ANY;    // automatically fill in my IP
        //    my_addr.sin_addr.s_addr = inet_addr("192.168.1.100");

        if (bind(m_sockfd, reinterpret_cast<sockaddr *>(&my_addr), sizeof(sockaddr)) == -1) {
            printf("ERROR--Socket bind failed! Port %d", m_port);
            return -1;
        }

        if (fcntl(m_sockfd, F_SETFL, O_NONBLOCK | FASYNC) < 0) {
            printf("ERROR--non-block! Port %d", m_port);
            return -1;
        }
        printf("INFO--Velodyne socket fd is  %d, port %d", m_sockfd, m_port);

    } else {
        m_readMode = READMODE::READNONE;
        ret = -1;
    }
    return ret; 
}

void CLidarGrabber::start() {

    m_readPacketThread = true;
    switch (m_readMode)
    {
        case READMODE::READFROMPCAP:
        {
            m_readPacketThreadHandle = new std::thread(&CLidarGrabber::readLidarPacketFromPcap, this);
            break;
        }
        case READMODE::READFROMSOCKET:
        {
            m_readPacketThreadHandle = new std::thread(&CLidarGrabber::readLidarPacketFromSocket, this);
            break;
        }
        default: break;
    }

    m_processPacketThreadHandle = new std::thread (&CLidarGrabber::processLidarPackets, this);
}

void CLidarGrabber::stop() {
    if (nullptr != m_readPacketThreadHandle) {
        m_readPacketThreadHandle->join();
        delete m_readPacketThreadHandle;
        m_readPacketThreadHandle = nullptr;
    }
    if (nullptr != m_processPacketThreadHandle)
    {
        m_processPacketThreadHandle->join();
        delete m_processPacketThreadHandle;
        m_processPacketThreadHandle = nullptr;
    }
    m_readPacketThread = false;
}

bool CLidarGrabber::isRunning() const { 
    return m_readPacketThread;
}

std::vector<PointXYZI> CLidarGrabber::getCurrentPacket()
{
    std::lock_guard<std::mutex> lock(m_vecCurrentPacketMutex);
    return m_vecCurrentPacket;
}

std::vector<PointXYZI> CLidarGrabber::getWholePacket()
{
    std::lock_guard<std::mutex> lock(m_vecWholePacketMutex);
    return m_vecWholePacket;
}

void CLidarGrabber::readLidarPacketFromPcap() {
    struct pcap_pkthdr *header;
    const std::uint8_t *data;
    std::int8_t errbuff[PCAP_ERRBUF_SIZE];

    pcap_t *pcap = pcap_open_offline(m_pcapFile.c_str (), reinterpret_cast<char*>(errbuff));

    struct bpf_program filter;

    // PCAP_NETMASK_UNKNOWN should be 0xffffffff, but it's undefined in older PCAP versions
    if (pcap_compile(pcap, &filter, "udp", 0, 0xffffffff) == -1) {
        printf("WARN--[CLidarGrabber::readPacketsFromPcap] Issue compiling filter: %s.\n", pcap_geterr(pcap));
    } else if (pcap_setfilter(pcap, &filter) == -1) {
        printf("WARN--[CLidarGrabber::readPacketsFromPcap] Issue setting filter: %s.\n", pcap_geterr(pcap));
    }

    struct timeval lasttime;

    lasttime.tv_sec = lasttime.tv_usec = 0;

    std::int32_t returnValue = pcap_next_ex(pcap, &header, &data);
    // int pkt_data = 0, pkt_position = 0;
    while (returnValue >= 0)
    {
        if (lasttime.tv_sec == 0) {
            lasttime.tv_sec = header->ts.tv_sec;
            lasttime.tv_usec = header->ts.tv_usec;
        }
        if (lasttime.tv_usec > header->ts.tv_usec) {
            lasttime.tv_usec -= 1000000;
            lasttime.tv_sec++;
        }
        std::uint64_t usec_delay = ((header->ts.tv_sec - lasttime.tv_sec) * 1000000) + (header->ts.tv_usec - lasttime.tv_usec);

        std::this_thread::sleep_for(std::chrono::microseconds(usec_delay));

        lasttime.tv_sec = header->ts.tv_sec;
        lasttime.tv_usec = header->ts.tv_usec;
        // if (header->len == 1248)        //data packet
        // {
        //     pkt_data++;
        //     printf("The %d packet length:%d;\n", pkt_data, header->len);
        // } else if (header->len == 554)  //position packet
        // {
        //     pkt_position++;
        //     printf("The %d position packet length:%d;\n", pkt_position, header->len);
        // }
        
        // The ETHERNET header is 42 bytes long; unnecessary
        enqueueLidarPacket(data + 42, header->len - 42);

        returnValue = pcap_next_ex(pcap, &header, &data);
    }

    m_readPacketThread = false;
}

void CLidarGrabber::readLidarPacketFromSocket() {
    std::uint8_t *data = nullptr;
    while (true) {
        if (!socketAvailable(1000)) {
            return ;
        }
        // Receive packets that should now be available from the
        // socket using a blocking read.
        ssize_t nbytes = recvfrom(m_sockfd, data, 1206, 0, nullptr, nullptr);

        if (nbytes < 0) {
            if (errno != EWOULDBLOCK) {
                printf("ERROR--recvfail from port %d", m_port);
                return ;
            }
        }

        if ((size_t)nbytes == 1206) {
            // read successful, done now
            enqueueLidarPacket(data, 1206);
            break;
        }
        printf("INFO--Incomplete Velodyne rising data packet read: %zd bytes from port %d", nbytes, m_port);
    }

    m_readPacketThread = false;
}

void CLidarGrabber::enqueueLidarPacket(const std::uint8_t *data, std::size_t bytesReceived) {
    if (bytesReceived == 1206)
    {
        uint8_t *dup = static_cast<std::uint8_t *>(malloc(bytesReceived * sizeof(std::uint8_t)));
        std::copy(data, data + bytesReceived, dup);
        std::lock_guard<std::mutex> lock(m_lidarPacketDataMutex);
        m_lidarPacketData.push(dup);
    }
}

void CLidarGrabber::processLidarPackets() {
    while (true)
    {
        if (m_readPacketThread)
        {
            if (!m_lidarPacketData.empty())
            {
                std::lock_guard<std::mutex> lock(m_lidarPacketDataMutex);
                std::uint8_t *data = m_lidarPacketData.front();
                m_lidarPacketData.pop();

                toPointClouds(data);
                // toPointClouds(reinterpret_cast<LaserDataPacket*>(data));

                free(data);
            }
            
        } else {
            break;
        }
        
    }
}

void CLidarGrabber::toPointClouds(LaserDataPacket *dataPacket) {
    std::uint32_t gpsTimestamp = dataPacket->gpsTimestamp;
    std::uint8_t mode = dataPacket->mode;
    std::uint8_t sensorType = dataPacket->sensorType;
    printf("-----------------------------------------\n");
    for (int i = 0; i < LASER_FIRING_PER_PKT; i++)
    {
        double azimuthInRadians = HDL_Grabber_toRadians((static_cast<double>(dataPacket->firingData[i].rotationalPosition) / 100.0));
        printf("\nblockIdentifier: %x\n", dataPacket->firingData[i].blockIdentifier);
        printf("rotationalPosition: %x, Radians:%f\n", dataPacket->firingData[i].rotationalPosition, azimuthInRadians);

        std::uint8_t offset = (dataPacket->firingData[i].blockIdentifier == BLOCK_0_TO_31) ? 0 : 32;
        for (int j = 0; j < LASER_PER_FIRING; j++)
        {
            printf(" -distance:%x", dataPacket->firingData[i].laserReturns[j].distance);
            printf(" -intensity:%x", dataPacket->firingData[i].laserReturns[j].intensity);
            PointXYZI point;
            computeXYZI(point, dataPacket->firingData[i].rotationalPosition, dataPacket->firingData[i].laserReturns[j], m_laser_corrections[j + offset]);
            printf(" -point.x:%f, point.y:%f, point.z:%f, point.i:%f\n", point.x, point.y, point.z, point.i);
        }
    }
    printf("\ngpsTimestamp: %x\n", gpsTimestamp);
    printf("mode: %x\n", mode);
    printf("sensorType: %x\n", sensorType);
    printf("-----------------------------------------\n");
}

void CLidarGrabber::toPointClouds(std::uint8_t *data)
{
    // printf("-----------------------------------------\n");
    LaserDataPacket lidarPacket;
    m_vecCurrentPacketMutex.lock();
    m_vecCurrentPacket.clear();
    m_vecCurrentPacketMutex.unlock();
    for (int i = 0; i < LASER_FIRING_PER_PKT; i++)
    {
        LaserFiringData firingData;
        std::uint8_t blockIdentifier[2], rotationalPosition[2];
        
        memcpy(&firingData.blockIdentifier, data + i * 100, 2);
        memcpy(&firingData.rotationalPosition, data + i * 100 + 2, 2);
        memcpy(blockIdentifier, data + i * 100, 2);
        memcpy(rotationalPosition, data + i * 100 + 2, 2);
        // printf("\nblockIdentifier: %x%x(%x)\n", blockIdentifier[0], blockIdentifier[1], firingData.blockIdentifier);
        // printf("rotationalPosition: %x%x(%x)\n", rotationalPosition[0], rotationalPosition[1], firingData.rotationalPosition);
        if (firingData.rotationalPosition < last_azimuth_)
        {
            // printf("******new scan********%x<%x\n", firingData.rotationalPosition, last_azimuth_);
            m_vecWholePacketMutex.lock();
            // printf("Point count: %d\n", m_vecWholePacket.size());
            if (sweep_xyzi_signal_ != nullptr && sweep_xyzi_signal_->num_slots () > 0)
                sweep_xyzi_signal_->operator()(m_vecWholePacket);
            m_vecWholePacket.clear();
            m_vecWholePacketMutex.unlock();
        }
        for (int j = 0; j < LASER_PER_FIRING; j++)
        {
            LaserReturn laserReturns;
            std::uint8_t distance[2], intensity;
            memcpy(&laserReturns.distance, data + i * 100 + 2 + 2 + j * 3, 2);
            memcpy(&laserReturns.intensity, data + i * 100 + 2 + 2 + j * 3 + 2, 1);
            memcpy(&distance, data + i * 100 + 2 + 2 + j * 3, 2);
            memcpy(&intensity, data + i * 100 + 2 + 2 + j * 3 + 2, 1);
            // printf(" -distance:%x%x(%x)", distance[0], distance[1], laserReturns.distance);
            // printf(" -intensity:%x(%x)", intensity, laserReturns.intensity);
            firingData.laserReturns[j] = laserReturns;
            PointXYZI point;
            computeXYZI(point, firingData.rotationalPosition, firingData.laserReturns[j], m_laser_corrections[j]);
            // printf(" -point.x:%f, point.y:%f, point.z:%f, point.i:%f\n", point.x, point.y, point.z, point.i);
            m_vecCurrentPacketMutex.lock();
            m_vecCurrentPacket.push_back(point);
            m_vecCurrentPacketMutex.unlock();
            m_vecWholePacketMutex.lock();
            m_vecWholePacket.push_back(point);
            m_vecWholePacketMutex.unlock();

        }
        lidarPacket.firingData[i] = firingData;
        last_azimuth_ = firingData.rotationalPosition;

    }
    std::uint8_t gpsTimestamp[4], mode, sensorType;
    memcpy(gpsTimestamp, data + LASER_FIRING_PER_PKT * 100, 4);
    memcpy(&mode, data + LASER_FIRING_PER_PKT * 100 + 4, 1);
    memcpy(&sensorType, data + LASER_FIRING_PER_PKT * 100 + 4 + 1, 1);

    memcpy(&lidarPacket.gpsTimestamp, data + LASER_FIRING_PER_PKT * 100, 4);
    memcpy(&lidarPacket.mode, data + LASER_FIRING_PER_PKT * 100 + 4, 1);
    memcpy(&lidarPacket.sensorType, data + LASER_FIRING_PER_PKT * 100 + 4 + 1, 1);

    // printf("\ngpsTimestamp: %x%x%x%x(%x)\n", gpsTimestamp[0], gpsTimestamp[1], gpsTimestamp[2], gpsTimestamp[3], lidarPacket.gpsTimestamp);
    // printf("mode: %x(%x)\n", mode, lidarPacket.mode);
    // printf("sensorType: %x(%x)\n", sensorType, lidarPacket.sensorType);
    // printf("-----------------------------------------\n");
}

bool CLidarGrabber::socketAvailable(int timeout) {
    struct pollfd fds[1];
    fds[0].fd = m_sockfd;
    fds[0].events = POLLIN;
    // Unfortunately, the Linux kernel recvfrom() implementation
    // uses a non-interruptible sleep() when waiting for data,
    // which would cause this method to hang if the device is not
    // providing data.  We poll() the device first to make sure
    // the recvfrom() will not block.
    //
    // Note, however, that there is a known Linux kernel bug:
    //
    //   Under Linux, select() may report a socket file descriptor
    //   as "ready for reading", while nevertheless a subsequent
    //   read blocks.  This could for example happen when data has
    //   arrived but upon examination has wrong checksum and is
    //   discarded.  There may be other circumstances in which a
    //   file descriptor is spuriously reported as ready.  Thus it
    //   may be safer to use O_NONBLOCK on sockets that should not
    //   block.

    // poll() until input available
    do {
        int retval = poll(fds, 1, 1000);

        if (retval < 0) {// poll() error?
            if (errno != EINTR) {
                printf("WARN--Velodyne port %d poll() error: %s", m_port, strerror(errno));
            }
            return false;
        }

        if (retval == 0) {// poll() timeout?
            printf("WARN--Velodyne port %d poll() timeout", m_port);
            return false;
        }

        if ((fds[0].revents & POLLERR) || (fds[0].revents & POLLHUP) ||
            (fds[0].revents & POLLNVAL)) {// device error?
            printf("ERROR--Velodyne port %d poll() reports Velodyne error", m_port);
            return false;
        }
    } while ((fds[0].revents & POLLIN) == 0);

    return true;
}

void CLidarGrabber::loadHDL32Corrections()
{
    double hdl32_vertical_corrections[] = { -30.67, -9.3299999, -29.33, -8, -28, -6.6700001, -26.67, -5.3299999, -25.33, -4, -24, -2.6700001, -22.67, -1.33, -21.33,
      0, -20, 1.33, -18.67, 2.6700001, -17.33, 4, -16, 5.3299999, -14.67, 6.6700001, -13.33, 8, -12, 9.3299999, -10.67, 10.67 };
    for (std::uint8_t i = 0; i < LASER_PER_FIRING; i++)
    {
        m_laser_corrections[i].azimuthCorrection = 0.0;
        m_laser_corrections[i].distanceCorrection = 0.0;
        m_laser_corrections[i].horizontalOffsetCorrection = 0.0;
        m_laser_corrections[i].verticalOffsetCorrection = 0.0;
        m_laser_corrections[i].verticalCorrection = hdl32_vertical_corrections[i];
        m_laser_corrections[i].sinVertCorrection = std::sin(HDL_Grabber_toRadians(hdl32_vertical_corrections[i]));
        m_laser_corrections[i].cosVertCorrection = std::cos(HDL_Grabber_toRadians(hdl32_vertical_corrections[i]));
    }
    for (std::uint8_t i = LASER_PER_FIRING; i < 64; i++)
    {
        m_laser_corrections[i].azimuthCorrection = 0.0;
        m_laser_corrections[i].distanceCorrection = 0.0;
        m_laser_corrections[i].horizontalOffsetCorrection = 0.0;
        m_laser_corrections[i].verticalOffsetCorrection = 0.0;
        m_laser_corrections[i].verticalCorrection = 0.0;
        m_laser_corrections[i].sinVertCorrection = 0.0;
        m_laser_corrections[i].cosVertCorrection = 1.0;
    }
}

void CLidarGrabber::computeXYZI(PointXYZI &point, std::uint16_t azimuth, LaserReturn laserReturn, HDLLaserCorrection correction)
{
    double cos_azimuth, sin_azimuth;

    double distanceM = laserReturn.distance * 0.002;

    point.i = static_cast<float>(laserReturn.intensity);

    if (correction.azimuthCorrection == 0)
    {
        cos_azimuth = cos_lookup_table_[azimuth];
        sin_azimuth = sin_lookup_table_[azimuth];
    }
    else
    {
        double azimuthInRadians = HDL_Grabber_toRadians((static_cast<double>(azimuth) / 100.0) - correction.azimuthCorrection);
        cos_azimuth = std::cos(azimuthInRadians);
        sin_azimuth = std::sin(azimuthInRadians);
    }

    distanceM += correction.distanceCorrection;

    double xyDistance = distanceM * correction.cosVertCorrection;

    point.x = static_cast<float>(xyDistance * sin_azimuth - correction.horizontalOffsetCorrection * cos_azimuth);
    point.y = static_cast<float>(xyDistance * cos_azimuth + correction.horizontalOffsetCorrection * sin_azimuth);
    point.z = static_cast<float>(distanceM * correction.sinVertCorrection + correction.verticalOffsetCorrection);
}

