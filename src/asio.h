#pragma once

#define ASIO_STANDALONE

#if defined (WIN32)
#pragma warning( push )
#pragma warning( disable : 4996 )
#define _WIN32_WINNT NTDDI_WINXP
#endif

#include <asio.hpp>

#if defined (WIN32)
#pragma warning( pop )
#endif

namespace pubsub {

asio::io_service& DefaultIoService();

}  // namespace pubsub
