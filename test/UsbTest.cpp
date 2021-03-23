/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2021  offa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mocks/LibUsbMocks.h"
#include <libusb-1.0/libusb.h>
#include <gmock/gmock.h>

using namespace testing;

namespace usb
{
    class UsbException : public std::exception
    {
    public:
        explicit UsbException(int errorCode)
            : error_(errorCode),
              name_(libusb_error_name(errorCode)),
              message_(libusb_strerror(errorCode))
        {
        }

        int code() const noexcept
        {
            return error_;
        }

        std::string name() const
        {
            return name_;
        }

        std::string message() const
        {
            return message_;
        }

        const char* what() const noexcept override
        {
            return message_.c_str();
        }

    private:
        int error_;
        std::string name_;
        std::string message_;
    };


    class Context
    {
    public:
        Context()
        {
            init();
        }

        Context(const Context&) = delete;

        ~Context()
        {
            deinit();
        }

        Context& operator=(const Context&) = delete;


    private:
        void init()
        {
            if (int status = libusb_init(nullptr); status != LIBUSB_SUCCESS)
            {
                throw UsbException{status};
            }
        }

        void deinit()
        {
            libusb_exit(nullptr);
        }
    };
}


class UsbTest : public testing::Test
{
protected:
    void SetUp() override
    {
        usbmock = mock::resetUsbMock();
    }

    void TearDown() override
    {
        mock::clearUsbMock();
    }

    mock::UsbMock* usbmock = nullptr;
};

TEST_F(UsbTest, contextCtorInitializesDefaultContext)
{
    EXPECT_CALL(*usbmock, init(nullptr)).WillOnce(Return(LIBUSB_SUCCESS));
    EXPECT_CALL(*usbmock, exit(_));

    usb::Context context;
}

TEST_F(UsbTest, contextCtorThrowsOnInitError)
{
    EXPECT_CALL(*usbmock, init(nullptr)).WillOnce(Return(LIBUSB_ERROR_OTHER));
    EXPECT_CALL(*usbmock, error_name(LIBUSB_ERROR_OTHER)).WillOnce(Return("ignore_name"));
    EXPECT_CALL(*usbmock, strerror(LIBUSB_ERROR_OTHER)).WillOnce(Return("ignore_message"));

    EXPECT_THROW(usb::Context{}, usb::UsbException);
}

TEST_F(UsbTest, contextDtorDeinitializesDefaultContext)
{
    EXPECT_CALL(*usbmock, init(_)).WillOnce(Return(LIBUSB_SUCCESS));
    EXPECT_CALL(*usbmock, exit(nullptr));

    usb::Context{};
}

TEST_F(UsbTest, exceptionContainsErrorInformation)
{
    EXPECT_CALL(*usbmock, error_name(_)).WillOnce(Return("name___LIBUSB_ERROR_NO_DEVICE"));
    EXPECT_CALL(*usbmock, strerror(_)).WillOnce(Return("message___LIBUSB_ERROR_NO_DEVICE"));

    const usb::UsbException ex{LIBUSB_ERROR_NO_DEVICE};
    EXPECT_THAT(ex.code(), Eq(LIBUSB_ERROR_NO_DEVICE));
    EXPECT_THAT(ex.name(), StrEq("name___LIBUSB_ERROR_NO_DEVICE"));
    EXPECT_THAT(ex.message(), StrEq("message___LIBUSB_ERROR_NO_DEVICE"));
    EXPECT_THAT(ex.what(), StrEq("message___LIBUSB_ERROR_NO_DEVICE"));
}
