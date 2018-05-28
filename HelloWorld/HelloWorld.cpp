#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

class HelloWorld : public eosio::contract {
    public:
    using contract::contract;

    /// @abi action
    void hello(const name user){
        print("Hello, ", name{user});
    }
};

EOSIO_ABI(HelloWorld, (hello))
