#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;
using std::string;

class hellostorage : public contract {
public:
  using contract::contract;

  [[eosio::action]]
  void hi(account_name user, string full_name){
    require_auth(user);
    print("Hello, ", name{user});
    addName(user, full_name);
  }

private:
  struct [[eosio::table]] entry{
    account_name user;
    string full_name;
    uint64_t primary_key() const {return user;}
  };

  typedef multi_index<N(history), entry> history_index;

  void addName(account_name user, string full_name){
    history_index history(_self, _self);
    auto iterator = history.find(user);
    eosio_assert(iterator == history.end(), "Name already exists");
    history.emplace(user, [&](auto& row){
      row.user = user;
      row.full_name = full_name;
    });
  }
};
EOSIO_ABI(hellostorage, (hi))
