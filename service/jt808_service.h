// Copyright 2019 Yuming Meng. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef JT808_SERVICE_JT808_SERVICE_H_
#define JT808_SERVICE_JT808_SERVICE_H_

#include <sys/epoll.h>
#include <string.h>

#include <list>
#include <string>
#include <vector>

#include "common/jt808_util.h"
#include "service/jt808_protocol.h"
#include "service/jt808_util.h"

class Jt808Service {
 public:
  Jt808Service() = default;
  Jt808Service(const Jt808Service&) = delete;
  Jt808Service& operator=(const Jt808Service&) = delete;
  virtual ~Jt808Service();

  // Init service.
  bool Init(const uint16_t &port, const int &max_count);
  bool Init(const char *ip, const uint16_t &port, const int &max_count);
  int AcceptNewClient(void);

  // Accept when command client connect.
  int AcceptNewCommandClient(void);

  int Jt808ServiceWait(const int &time_out);
  void Run(const int &time_out);

  int SendFrameData(const int &fd, const Message &msg);
  int RecvFrameData(const int &fd, Message *msg);

  size_t Jt808FramePack(const uint16_t &command,
                        const ProtocolParameters &propara, Message *msg);

  uint16_t Jt808FrameParse(Message *msg, ProtocolParameters *propara);

  int DealGetTerminalParameterRequest(DeviceNode *device,
                                      std::vector<std::string> *va_vec);
  int DealSetTerminalParameterRequest(DeviceNode *device,
                                      std::vector<std::string> *va_vec);
  int DealSetCircularAreaRequest(DeviceNode *device,
                                 std::vector<std::string> *va_vec);
  int DealSetRectangleAreaRequest(DeviceNode *device,
                                  std::vector<std::string> *va_vec);
  int DealSetPolygonalAreaRequest(DeviceNode *device,
                                  std::vector<std::string> *va_vec);
  int DealSetRouteRequest(DeviceNode *device, std::vector<std::string> *va_vec);
  int DealDeleteAreaRouteRequest(DeviceNode *device,
                                 std::vector<std::string> *va_vec,
                                 const uint16_t &command);
  int DealGetPositionInfoRequest(DeviceNode *device);
  int DealPositionTrackRequest(DeviceNode *device,
                               std::vector<std::string> *va_vec);
  int DealTerminalControlRequest(DeviceNode *device,
                                 std::vector<std::string> *va_vec);
  int DealVehicleControlRequest(DeviceNode *device,
                                std::vector<std::string> *va_vec);

  int ParseCommand(char *command);
  bool CheckPacketComplete(int *sock, ProtocolParameters *propara);

  // Deal upgrade request thread.
  void UpgradeHandler(void);
  static void *StartUpgradeThread(void *arg) {
    Jt808Service *_this = reinterpret_cast<Jt808Service *>(arg);
    _this->UpgradeHandler();
    return nullptr;
  }

 private:
  const char *kDevicesFilePath = "/etc/jt808/service/devices.txt";
  const char *kCommandInterfacePath = "/tmp/jt808cmd.sock";

  int listen_sock_ = -1;
  int epoll_fd_ = -1;
  int max_count_ = 0;
  uint16_t message_flow_num_ = 0;
  int socket_fd_ = -1;
  int client_fd_ = -1;
  uid_t uid_;
  char file_path[256] = {0};
  std::list<DeviceNode *> device_list_;
  struct epoll_event *epoll_events_ = nullptr;
};

#endif  // JT808_SERVICE_JT808_SERVICE_H_
