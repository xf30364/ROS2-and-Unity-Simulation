/**
 * @file Basic.cpp
 * @brief 基本数据类型定义
 * @author BG2EDG (928330305@qq.com)
 * @date 2022-04-20
 *
 * @copyright Copyright (C) 2022, HITCRT_VISION, all rights reserved.
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Author  <th>Description
 * <tr><td>2022-04-20 <td>BG2EDG  <td>
 * </table>
 */
#include "Basic.h"

namespace hitcrt {

RecvInfoBase::RecvInfoBase(const float pitch, const float yaw, const float roll, const float firingSpeed,
                           const Color enemyColor, const bool useVision)
    : m_pitch(pitch),
      m_yaw(yaw),
      m_roll(roll),
      m_firingSpeed(firingSpeed),
      m_enemyColor(enemyColor),
      m_useVision(useVision) {}
// void RecvInfo::set(const float pitch, const float yaw, const float
// firingSpeed,
//                    const Color enemyColor, const bool useVision) {
//     m_pitch = pitch;
//     m_yaw = yaw;
//     m_firingSpeed = firingSpeed;
//     m_enemyColor = enemyColor;
//     m_useVision = useVision;
// }

// boost::any &RecvInfo::operator[](const std::string &memberName) {
//     auto iter = m_recvList.find(memberName);
//     try {
//         if (iter == m_recvList.end()) {
//             throw EmptyNameError();
//         }
//     } catch (EmptyNameError &emptyNameError) {
//         std::cerr << emptyNameError.what() << std::endl;
//     }

//     return iter->second;
// }

// void RecvInfo::operator<<(const std::string &memberName)
// {
//     m_recvList.emplace(memberName);
// }

// void RecvInfo::add(const std::string &memberName, const boost::any &member) {
//     m_recvList.emplace(std::make_pair(memberName, member));
// }

// void TestRecvInfo::SetUp() {
//     m_recvInfo.set(10.0, 20.0, 28000, Color::BLUE, true);
// }

// void TestRecvInfo::addMemberTesting() {
//     // 设值成功
//     // m_recvInfo.add("flag", false);
//     // auto flag = boost::any_cast<bool>(m_recvInfo["flag"]);
//     // ASSERT_TRUE(flag);

//     // 数据类型不对会抛出异常
//     // m_recvInfo.add("pitch", 10.0);
//     // double flag = boost::any_cast<double>(m_recvInfo["pitch"]);
//     // std::cout << "pitch double: " << flag;
//     // float flag2 = boost::any_cast<float>(m_recvInfo["pitch"]);
//     // std::cout << " pitch float: " << flag2 << std::endl;
// }

// void TestRecvInfo::getTesting() {
//     // 直接找根本不耗时，但是用boost::any_cast和map找东西则会与大小和次数有关
//     // 想办法从设计上解决
//     // m_recvInfo.add("pitch", 10.0);
//     double yaw = boost::any_cast<double>(m_recvInfo["yaw"]);

//     auto start = std::chrono::system_clock::now();
//     for (uint i = 0; i < 1000000000; i++) {
//         double flag = boost::any_cast<double>(m_recvInfo["pitch"]);
//     }
//     auto end = std::chrono::system_clock::now();
//     auto duration =
//         std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//     std::cout << "Any Get Cost: "
//               << double(duration.count()) *
//                      std::chrono::microseconds::period::num /
//                      std::chrono::microseconds::period::den
//               << " seconds." << std::endl;
//     start = std::chrono::system_clock::now();
//     for (uint i = 0; i < 1000000000; i++) {
//         double flag = m_recvInfo.m_pitch;
//     }
//     end = std::chrono::system_clock::now();
//     duration =
//         std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//     std::cout << "Normal Get Cost: "
//               << double(duration.count()) *
//                      std::chrono::microseconds::period::num /
//                      std::chrono::microseconds::period::den
//               << " seconds." << std::endl;
// }
}  // namespace hitcrt