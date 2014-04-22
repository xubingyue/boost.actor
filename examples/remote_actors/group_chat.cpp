/******************************************************************************\
 * This example program represents a minimal terminal chat program            *
 * based on group communication.                                              *
 *                                                                            *
 * Setup for a minimal chat between "alice" and "bob":                        *
 * - ./build/bin/group_server -p 4242                                         *
 * - ./build/bin/group_chat -g remote:chatroom@localhost:4242 -n alice        *
 * - ./build/bin/group_chat -g remote:chatroom@localhost:4242 -n bob          *
\******************************************************************************/

#include <set>
#include <map>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <iostream>

#include "boost/program_options.hpp"

#include "boost/actor/cppa.hpp"

using namespace std;
using namespace boost::actor;
using namespace boost::actor::placeholders;
using namespace boost::program_options;

struct line { string str; };

istream& operator>>(istream& is, line& l) {
    getline(is, l.str);
    return is;
}

namespace { string s_last_line; }

any_tuple split_line(const line& l) {
    istringstream strs(l.str);
    s_last_line = move(l.str);
    string tmp;
    vector<string> result;
    while (getline(strs, tmp, ' ')) {
        if (!tmp.empty()) result.push_back(std::move(tmp));
    }
    return any_tuple::view(std::move(result));
}

void client(event_based_actor* self, const string& name) {
    self->become (
        on(atom("broadcast"), arg_match) >> [=](const string& message) {
            for(auto& dest : self->joined_groups()) {
                self->send(dest, name + ": " + message);
            }
        },
        on(atom("join"), arg_match) >> [=](const group& what) {
            for (auto g : self->joined_groups()) {
                cout << "*** leave " << to_string(g) << endl;
                self->send(self, g, name + " has left the chatroom");
                self->leave(g);
            }
            cout << "*** join " << to_string(what) << endl;
            self->join(what);
            self->send(what, name + " has entered the chatroom");
        },
        [=](const string& txt) {
            // don't print own messages
            if (self->last_sender() != self) cout << txt << endl;
        },
        [=](const group_down_msg& g) {
            cout << "*** chatroom offline: " << to_string(g.source) << endl;
        },
        others() >> [=]() {
            cout << "unexpected: " << to_string(self->last_dequeued()) << endl;
        }
    );
}

int main(int argc, char** argv) {
    string name;
    string group_id;
    options_description desc("Allowed options");
    desc.add_options()
        ("name,n", value<string>(&name), "set name")
        ("group,g", value<string>(&group_id), "join group")
        ("help,h", "print help")
    ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
        cout << desc << endl;
        return 1;
    }

    while (name.empty()) {
        cout << "please enter your name: " << flush;
        if (!getline(cin, name)) {
            cerr << "*** no name given... terminating" << endl;
            return 1;
        }
    }

    auto client_actor = spawn(client, name);

    // evaluate group parameters
    if (!group_id.empty()) {
        auto p = group_id.find(':');
        if (p == std::string::npos) {
            cerr << "*** error parsing argument " << group_id
                 << ", expected format: <module_name>:<group_id>";
        }
        else {
            try {
                auto g = group::get(group_id.substr(0, p),
                                    group_id.substr(p + 1));
                anon_send(client_actor, atom("join"), g);
            }
            catch (std::exception& e) {
                ostringstream err;
                cerr << "*** exception: group::get(\"" << group_id.substr(0, p)
                     << "\", \"" << group_id.substr(p + 1) << "\") failed; "
                     << to_verbose_string(e) << endl;
            }
        }
    }

    cout << "*** starting client, type '/help' for a list of commands" << endl;
    istream_iterator<line> lines(cin);
    istream_iterator<line> eof;
    /*
    match_each (lines, eof, split_line) (
        on("/join", arg_match) >> [&](const string& mod, const string& id) {
            try {
                anon_send(client_actor, atom("join"), group::get(mod, id));
            }
            catch (std::exception& e) {
                cerr << "*** exception: " << to_verbose_string(e) << endl;
            }
        },
        on("/quit") >> [&] {
            // close STDIN; causes this match loop to quit
            cin.setstate(ios_base::eofbit);
        },
        on<string, anything>().when(_x1.starts_with("/")) >> [&] {
            cout <<  "*** available commands:\n"
                     "    /join <module> <group> join a new chat channel\n"
                     "    /quit                  quit the program\n"
                     "    /help                  print this text\n" << flush;
        },
        others() >> [&] {
            if (!s_last_line.empty()) {
                anon_send(client_actor, atom("broadcast"), s_last_line);
            }
        }
    );
    */
    // force actor to quit
    anon_send_exit(client_actor, exit_reason::user_shutdown);
    await_all_actors_done();
    shutdown();
    return 0;
}
