#ifndef _HELLOWORLDIMG_H_
#define _HELLOWORLDIMG_H_

#include <dds/core/ddscore.hpp>

namespace HelloWorldImg
{
    class RGB OSPL_DDS_FINAL
    {
    public:
        uint8_t r_;
        uint8_t g_;
        uint8_t b_;

    public:
        RGB() :
                r_(0),
                g_(0),
                b_(0) {}

        explicit RGB(
            uint8_t r,
            uint8_t g,
            uint8_t b) : 
                r_(r),
                g_(g),
                b_(b) {}

        RGB(const RGB &_other) : 
                r_(_other.r_),
                g_(_other.g_),
                b_(_other.b_) {}

#ifdef OSPL_DDS_CXX11
        RGB(RGB &&_other) : 
                r_(::std::move(_other.r_)),
                g_(::std::move(_other.g_)),
                b_(::std::move(_other.b_)) {}

        RGB& operator=(RGB &&_other)
        {
            if (this != &_other) {
                r_ = ::std::move(_other.r_);
                g_ = ::std::move(_other.g_);
                b_ = ::std::move(_other.b_);
            }
            return *this;
        }
#endif

        RGB& operator=(const RGB &_other)
        {
            if (this != &_other) {
                r_ = _other.r_;
                g_ = _other.g_;
                b_ = _other.b_;
            }
            return *this;
        }

        bool operator==(const RGB& _other) const
        {
            return r_ == _other.r_ &&
                g_ == _other.g_ &&
                b_ == _other.b_;
        }

        bool operator!=(const RGB& _other) const
        {
            return !(*this == _other);
        }

        uint8_t r() const { return this->r_; }
        uint8_t& r() { return this->r_; }
        void r(uint8_t _val_) { this->r_ = _val_; }
        uint8_t g() const { return this->g_; }
        uint8_t& g() { return this->g_; }
        void g(uint8_t _val_) { this->g_ = _val_; }
        uint8_t b() const { return this->b_; }
        uint8_t& b() { return this->b_; }
        void b(uint8_t _val_) { this->b_ = _val_; }
    };

    const int16_t SIZE_HORIZONTAL = 224;

    const int16_t SIZE_VERTICAL = 224;

    class Image OSPL_DDS_FINAL
    {
    public:
        typedef dds::core::array<dds::core::array<HelloWorldImg::RGB, 224> , 224>  _matrix;

        int32_t imgID_;
        std::string category_;
        dds::core::array<dds::core::array<HelloWorldImg::RGB, 224> , 224>  matrix_;

    public:
        Image() :
                imgID_(0) {}

        explicit Image(
            int32_t imgID,
            const std::string& category,
            const dds::core::array<dds::core::array<HelloWorldImg::RGB, 224> , 224> & matrix) : 
                imgID_(imgID),
                category_(category),
                matrix_(matrix) {}

        Image(const Image &_other) : 
                imgID_(_other.imgID_),
                category_(_other.category_),
                matrix_(_other.matrix_) {}

#ifdef OSPL_DDS_CXX11
        Image(Image &&_other) : 
                imgID_(::std::move(_other.imgID_)),
                category_(::std::move(_other.category_)),
                matrix_(::std::move(_other.matrix_)) {}

        Image& operator=(Image &&_other)
        {
            if (this != &_other) {
                imgID_ = ::std::move(_other.imgID_);
                category_ = ::std::move(_other.category_);
                matrix_ = ::std::move(_other.matrix_);
            }
            return *this;
        }
#endif

        Image& operator=(const Image &_other)
        {
            if (this != &_other) {
                imgID_ = _other.imgID_;
                category_ = _other.category_;
                matrix_ = _other.matrix_;
            }
            return *this;
        }

        bool operator==(const Image& _other) const
        {
            return imgID_ == _other.imgID_ &&
                category_ == _other.category_ &&
                matrix_ == _other.matrix_;
        }

        bool operator!=(const Image& _other) const
        {
            return !(*this == _other);
        }

        int32_t imgID() const { return this->imgID_; }
        int32_t& imgID() { return this->imgID_; }
        void imgID(int32_t _val_) { this->imgID_ = _val_; }
        const std::string& category() const { return this->category_; }
        std::string& category() { return this->category_; }
        void category(const std::string& _val_) { this->category_ = _val_; }
#ifdef OSPL_DDS_CXX11
        void category(std::string&& _val_) { this->category_ = _val_; }
#endif
        const dds::core::array<dds::core::array<HelloWorldImg::RGB, 224> , 224> & matrix() const { return this->matrix_; }
        dds::core::array<dds::core::array<HelloWorldImg::RGB, 224> , 224> & matrix() { return this->matrix_; }
        void matrix(const dds::core::array<dds::core::array<HelloWorldImg::RGB, 224> , 224> & _val_) { this->matrix_ = _val_; }
#ifdef OSPL_DDS_CXX11
        void matrix(dds::core::array<dds::core::array<HelloWorldImg::RGB, 224> , 224> && _val_) { this->matrix_ = _val_; }
#endif
    };

}

#endif /* _HELLOWORLDIMG_H_ */
