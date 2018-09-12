#include <gtest/gtest.h>
#include "core.h"

using namespace pubsub;

TEST(CoreTest, StartupShutdownOk) {
  ASSERT_NO_THROW(InitializePubsub());
  ASSERT_NO_THROW(ShutdownPubsub());
}