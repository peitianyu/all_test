#include"core/tt_test.h"
#include"common/fsm.h"


void spring_thing(){ printf("this is spring\n"); }
void summer_thing(){ printf("this is summer\n"); }
void autumn_thing(){ printf("this is autumn\n"); }
void winter_thing(){ printf("this is winter\n"); }



// JUST_RUN_TEST(fsm, test1)
TEST(fsm, test1)
{
    Fsm fsm(SPRING);
    fsm.AddTable({ EVENT1,  SPRING,    summer_thing,  SUMMER });
    fsm.AddTable({ EVENT2,  SUMMER,    autumn_thing,  AUTUMN });
    fsm.AddTable({ EVENT3,  AUTUMN,    winter_thing,  WINTER });
    fsm.AddTable({ EVENT4,  WINTER,    spring_thing,  SPRING });

    fsm.EventHandle(EVENT1);
    fsm.EventHandle(EVENT2);
    fsm.EventHandle(EVENT3);
    fsm.EventHandle(EVENT4);
    fsm.EventHandle(EVENT1);
    fsm.EventHandle(EVENT3);
}

std::vector<FsmTable_t> year_table =
{
    { EVENT1,  SPRING,    summer_thing,  SUMMER },
    { EVENT2,  SUMMER,    autumn_thing,  AUTUMN },
    { EVENT3,  AUTUMN,    winter_thing,  WINTER },
    { EVENT4,  WINTER,    spring_thing,  SPRING },
};
// JUST_RUN_TEST(fsm, test2)
TEST(fsm, test2)
{
    Fsm fsm(SPRING, year_table);
    fsm.EventHandle(EVENT1);
    fsm.EventHandle(EVENT2);
    fsm.EventHandle(EVENT3);
    fsm.EventHandle(EVENT4);
    fsm.EventHandle(EVENT1);
    fsm.EventHandle(EVENT3);
}