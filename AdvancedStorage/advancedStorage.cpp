#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;
using std::string;

class advstorage : public contract {
public:
  using contract::contract;

  [[eosio::action]]
  void hi(account_name user, string full_name){
    require_auth(user);
    print("Hello, ", name{user});
    addName(user, full_name);
  }

  [[eosio::action]]
  void forget(account_name user){
    require_auth(user);
    removeName(user);
  }

private:
  struct [[eosio::table]] entry{
    account_name user;
    string full_name;
    uint64_t lastUpdate;
    uint64_t primary_key() const {return user;}
  };

  //Configuration
  typedef multi_index<N(advhistory), entry> history_index;

  void addName(account_name user, string full_name){
    history_index history(_self, _self);
    auto iterator = history.find(user);
    if(iterator == history.end()){
      //User doesn't exist in table
      history.emplace(user, [&](auto& row){
        row.user = user;
        row.full_name = full_name;
        row.lastUpdate = now();
      });
    }
    else{
      //User already exists in table, let's update
      history.modify(iterator, user, [&](auto& row){
        row.user = user;
        row.full_name = full_name;
        row.lastUpdate = now();
      });
    }
  }

  void removeName(account_name user){
    history_index history(_self, _self);
    auto iterator = history.find(user);
    eosio_assert(iterator != history.end(), "User does not exist");
    history.erase(iterator);
  }
};
EOSIO_ABI(advstorage, (hi)(forget))
