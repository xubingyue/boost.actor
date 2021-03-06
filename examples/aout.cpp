/******************************************************************************\
 * This example illustrates how to use aout.                                  *
\******************************************************************************/

#include <random>
#include <chrono>
#include <cstdlib>
#include <iostream>

#include "boost/actor/all.hpp"

using boost::none;
using boost::join;
using boost::variant;
using boost::optional;
using boost::is_any_of;
using boost::token_compress_on;
using namespace boost::actor;
using std::endl;

int main() {
  for (int i = 1; i <= 50; ++i) {
    spawn<blocking_api>([i](blocking_actor* self) {
      aout(self) << "Hi there! This is actor nr. "
                 << i << "!" << endl;
      std::random_device rd;
      std::default_random_engine re(rd());
      std::chrono::milliseconds tout{re() % 10};
      self->delayed_send(self, tout, 42);
      self->receive(
        [i, self](int) {
          aout(self) << "Actor nr. "
                     << i << " says goodbye!" << endl;
        }
      );
    });
  }
  // wait until all other actors we've spawned are done
  await_all_actors_done();
  // done
  shutdown();
}
