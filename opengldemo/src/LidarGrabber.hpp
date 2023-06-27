
#include <string>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <boost/signals2.hpp>

using namespace std;

enum READMODE{
    READFROMPCAP = 0,
    READFROMSOCKET = 1,
    READNONE = 2
};

// #define LASER_PER_FIRING (32)
// #define LASER_FIRING_PER_PKT (12)

#define HDL_Grabber_toRadians(x) ((x) * M_PI / 180.0)


struct PointXYZI
{
    float x;
    float y;
    float z;
    float i;
};

class CLidarGrabber{
public:

    using sig_sweep_point_cloud_xyzi = void (const std::vector<PointXYZI> &);

    CLidarGrabber() = default;
    CLidarGrabber(std::string pcapFile);
    CLidarGrabber(std::string ipAddress, int port);
    CLidarGrabber(std::string pcapFile, std::string ipAddress, int port = 2368);
    virtual ~CLidarGrabber();

    int lidarGrabberInit();

    void start();
    void stop();

    template<typename T>
    void registerCallback(const std::function<T> & callback);

    bool isRunning() const;

    std::vector<PointXYZI> getCurrentPacket();
    std::vector<PointXYZI> getWholePacket();

    enum HDLBlock
    {
        BLOCK_0_TO_31 = 0xeeff, BLOCK_32_TO_63 = 0xddff
    };
protected:

    static const std::uint8_t LASER_PER_FIRING = 32;
    static const std::uint8_t LASER_FIRING_PER_PKT = 12;
    struct LaserReturn
    {
        std::uint16_t distance;
        std::uint8_t intensity;
    };

    struct LaserFiringData
    {
        std::uint16_t blockIdentifier;
        std::uint16_t rotationalPosition;
        LaserReturn laserReturns[LASER_PER_FIRING];
    };

    struct LaserDataPacket
    {
        LaserFiringData firingData[LASER_FIRING_PER_PKT];
        std::uint32_t gpsTimestamp;
        std::uint8_t mode;
        std::uint8_t sensorType;
    };

    struct HDLLaserCorrection
    {
        double azimuthCorrection;
        double verticalCorrection;
        double distanceCorrection;
        double verticalOffsetCorrection;
        double horizontalOffsetCorrection;
        double sinVertCorrection;
        double cosVertCorrection;
        double sinVertOffsetCorrection;
        double cosVertOffsetCorrection;
    };
    boost::signals2::signal<sig_sweep_point_cloud_xyzi>* sweep_xyzi_signal_;
private:
    void readLidarPacketFromPcap();
    void readLidarPacketFromSocket();
    void enqueueLidarPacket(const std::uint8_t *data, std::size_t bytesReceived);
    void processLidarPackets();
    void toPointClouds(LaserDataPacket *dataPacket);
    void toPointClouds(std::uint8_t *data);

    bool socketAvailable(int timeout);

    void loadHDL32Corrections();

    void computeXYZI(PointXYZI &point, std::uint16_t azimuth, LaserReturn laserReturn, HDLLaserCorrection correction);

    std::string m_pcapFile;
    std::string m_ipAddress;
    int m_port;

    int m_sockfd;

    READMODE m_readMode;

    std::thread *m_readPacketThreadHandle;
    std::thread *m_processPacketThreadHandle;

    bool m_readPacketThread = false;

    std::queue<uint8_t*> m_lidarPacketData;
    std::mutex m_lidarPacketDataMutex;

    HDLLaserCorrection m_laser_corrections[64];

    static const std::uint16_t HDL_NUM_ROT_ANGLES = 36001;
    static double *cos_lookup_table_;
    static double *sin_lookup_table_;

    std::uint16_t last_azimuth_;

    float min_distance_threshold_;
    float max_distance_threshold_;

    std::vector<PointXYZI> m_vecWholePacket;
    std::mutex m_vecWholePacketMutex;
    std::vector<PointXYZI> m_vecCurrentPacket;
    std::mutex m_vecCurrentPacketMutex;

};

template <typename T>
inline void CLidarGrabber::registerCallback(const std::function<T> &callback)
{
    if (nullptr != sweep_xyzi_signal_)
    {
        sweep_xyzi_signal_->connect(callback);
    }
}
