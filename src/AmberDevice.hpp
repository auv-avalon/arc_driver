#ifndef _ARCDRIVER_AMBER_DEVICE_HPP_
#define _ARCDRIVER_AMBER_DEVICE_HPP_
namespace arc_driver
{
    class AmberDevice
    {
        public: 
            AmberDevice(int fd);
            void enterCommandMode();
            void leaveCommandMode();
        private:
            int fd;
    };

} // end namespace arc_driver
#endif
