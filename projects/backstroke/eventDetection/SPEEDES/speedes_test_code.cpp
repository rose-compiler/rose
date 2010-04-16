
void DefineEvent(void* p)
{
}

#define DEFINE_SIMOBJ_EVENT_1_ARG(name, cls, func, arg) DefineEvent(0);
#define DEFINE_SIMOBJ_EVENT_0_ARG(name, cls, func) DefineEvent(0);

class SpSimObj {};
class S_Car : public SpSimObj {
  public:
    S_Car() {};
    virtual ~S_Car() {};

    virtual void Init();
    void         Stop(double stopTime);
    void         Go();
    void         StopCar();

};

int main(){
//DEFINE_SIMOBJ(S_Car, 4, SCATTER);
DEFINE_SIMOBJ_EVENT_1_ARG(Car_Stop,S_Car,Stop,double);
DEFINE_SIMOBJ_EVENT_0_ARG(Car_Go, S_Car, Go);
DEFINE_SIMOBJ_EVENT_0_ARG(Car_StopCar, S_Car, StopCar);
}
