#include "llama-cpp.hpp"

int main(int argc, char **argv)
{
    LlamaCpp llamaCpp;

    llamaCpp.init(1);
    llamaCpp.load_model("../h2o-danube3-500m-base-q4_k_m.gguf");

    std::string prompt = "Hello my name is";

    std::string pred = llamaCpp.predict(prompt);

    std::string out = prompt + pred;

    printf("%s\n", out.c_str());

    llamaCpp.destroy();

    return 0;
}