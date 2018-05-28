#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/singleton.hpp>

using namespace eosio;
using std::string;

class HelloSingleton : public eosio::contract {
public:
    using contract::contract;

    /// @abi action
    void hello(const name username, const string& full_name){
        require_auth(username);
        print("Hello, ", name{username});
        addEntry(username, full_name);
    }

    /// @abi action
    void setowner(const name newOwner){
        owner own(_self, _self);
        if(own.exists()){
            require_auth(own.get());
        }
        else{
            require_auth(_self);
        }
        require_auth(newOwner);
        own.set(newOwner, newOwner);
    }

private:
    /// @abi table log i64
    struct log{
        uint64_t username;
        string full_name;
        uint64_t primary_key() const {return username;}

        EOSLIB_SERIALIZE(log, (username)(full_name));
    };


    typedef multi_index<N(log), log> helloIndex;
    typedef singleton<N(HelloS), name> owner;

    void addEntry(const name username, const string& full_name){
        helloIndex entries(_self, _self);
        auto iterator = entries.find(username);
        eosio_assert(iterator == entries.end(), "Name already exists");
        entries.emplace(username, [&](auto& logEntry){
            logEntry.username = username;
            logEntry.full_name = full_name;
        });
    }

};

EOSIO_ABI(HelloSingleton, (hello)(setowner))
