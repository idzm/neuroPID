#ifndef NN_MANAGER
#define NN_MANAGER

#include <vector>

#include "mlp.h"
#include "plant.h"
#include "PID.h"

class nn_manager
{
public:

    nn_manager(int time_interval,
        int inputs_count_per_parameter);

    ~nn_manager();


    plant* get_plant() const;

    PID* get_PID() const;

    void set_learning(bool use_learning);

    void eval();


    std::vector<float>* get_plant_data();

    std::vector<float>* get_PID_data();

    std::vector<float>* get_nn2_emul_data();

private:
    int time_interval;

    std::vector<float>* p_plant_data;
    std::vector<float>* PID_data;
    std::vector<float>* nn2_out;
    std::vector<float*>* nn2_in;

    plant* p_plant;        ///Установка.
    mlp* nn2_emulator;    ///Нейронная сеть.
    PID* p_pid;           ///ПИД.
    mlp* nn1_tuner;       ///Нейронная сеть.

    FILE* data_stream;

    rt_sample* emul_sample;
    rt_sample* tuner_sample;

    rt_sample* tuner_future_sample;

    bool use_learning;
};
#endif //NN_MANAGER
