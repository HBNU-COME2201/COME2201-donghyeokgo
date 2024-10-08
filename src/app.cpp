#include <iostream>
#include "agent.hpp"
#include "mmanager.hpp"
#include "stochastic_decorator.hpp"
#include "simple_decorator.hpp"

#include <vector>
#include <string>
#include "tinyxml2.h"
#include "special_agent.hpp"

// 과제2.app.cpp 주석 달기
//     - InitManager 주석
//      클라이언트 코드에 왜 클래스 타입을 몰라도 동작하는 지 설명 (힌트: 다형성)

/*

    다형성 : 동일한 인터페이스나 기본 클래스를 통해 여러 다른 타입의 객체를 다룰 수 있게 해주는 oop 핵심 개념
    다형성 구현 방법
                -->  1. 상속, 메소드 오버라이딩 : 부모 클래스의 메소드를 자식 클래스에서 재정의
                    2. 인터페이스 구현        : 여러 클래스가 동일한 인터페이스를 구현해 다형성 실현
                    3. 제네릭 프로그래밍      : 타입에 독립적인 코드를 작성할 수 있게 해줌

    (상속, 메소드 오버라이딩)
    m_agent_list 는 CAgent* 포인터를 저장 --> 파생 클래스 CSpecialAgent를 다룰 수 있음
            --> CSpecialAgent는 CAgent의 자식 클래스이므로 CAgent의 함수 실행 가능

    (인터페이스 구현)
    CManeuverManager 클래스 --> CAgent 인터페이스를 통해 모든 에이전트를 접근
            --> 구체적인 에이전트 타입을 몰라도 사용, 동작 가능

*/

class InitManager
{
public:
    InitManager() {}
    // 생성자 XML 파일 경로에 있는 Agent정보 초기화 함 (test.xml)
    InitManager(std::string path)
    {

        tinyxml2::XMLDocument doc;
        doc.LoadFile(path.c_str());

        // 요소 찾기 XML, scenario -> AgentList객체 호출
        tinyxml2::XMLElement *agListElem = doc.FirstChildElement("scenario")->FirstChildElement("AgentList");

        double x, y, heading, speed, drange;
        // 요소 x, y, heading, speed, drange 찾음
        for (tinyxml2::XMLElement *agElem = agListElem->FirstChildElement();
             agElem != NULL; agElem = agElem->NextSiblingElement())
        {
            // xml에서 각 에이전트의 속성을 읽음
            agElem->QueryDoubleAttribute("x", &x);
            agElem->QueryDoubleAttribute("y", &y);
            agElem->QueryDoubleAttribute("heading", &heading);
            agElem->QueryDoubleAttribute("speed", &speed);
            agElem->QueryDoubleAttribute("drange", &drange);

            // CSpecialAgent객체 생성
            CAgent *ag = new CSpecialAgent(x, y, heading, speed, drange);
            // 리스트에 추가
            m_agent_list.push_back(ag);
        }
    }

private:
    // CAget* 타입의 포인터를 저장하는 벡터 m_agent_list
    // CAgent의 파생클래스 CSpecialAgent의 정보를 저장
    std::vector<CAgent *> m_agent_list;

    // 에이전트 리스트 접근자
public:
    std::vector<CAgent *> &get_agent_list() { return m_agent_list; }
};

// 클라이언트 코드

int main(int argc, char **argv)
{
    // test.xml에서 에이전트 초기화
    InitManager init_manager("test.xml");
    /* Simulation Engine */
    CManeuverManager mmanager;

    // 모든 에이전트 객체를 CManeuverManager에 등록
    for (std::vector<CAgent *>::iterator iter = init_manager.get_agent_list().begin();
         iter != init_manager.get_agent_list().end(); ++iter)
    {
        mmanager.register_publisher(*iter);
    }

    /* Simulation Engine Initialization */
    double sim_time;
    double time_step = 1;

    for (sim_time = 0; sim_time < 30; sim_time += time_step)
    {

        mmanager.svc(time_step);

        // 현재 시간, 에이전트 위치, 상태 출력
        std::cout << "----" << std::endl;
        for (std::vector<CAgent *>::iterator iter = init_manager.get_agent_list().begin();
             iter != init_manager.get_agent_list().end(); ++iter)
        {
            std::cout << "Time: " << sim_time << ", " << *(*iter) << std::endl;
        }
    }
    return 0;
}
