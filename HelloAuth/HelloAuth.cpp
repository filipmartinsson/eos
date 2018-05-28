#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

class HelloAuth : public eosio::contract {
    public:
    using contract::contract;

    /// @abi action
    void hello(const name user){
        require_auth(user);
        print("Hello, ", name{user});
    }
};

EOSIO_ABI(HelloAuth, (hello))
