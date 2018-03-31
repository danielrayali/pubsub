#pragma once

namespace pubsub {

class MasterServer {
 public:
   MasterServer() = default;

   ~MasterServer() = default;

   void Run();

   void Stop();
};

}  // namespace pubsub
