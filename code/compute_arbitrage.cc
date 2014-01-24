# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <map>
# include <set>
# include <queue>
# include <stack>

using namespace std;

# define EPS 1.0

struct transit {
  string city;
  vector<string> stations;
  map<string, vector<string> > connections;
  map<string, map<string, double> > fares;

  transit(string city) {
    this->city = city;
  }

  void read_data() {
    string prefix = "../data/" + city;

    // stations
    string stations_file = prefix + "/stations.txt";
    ifstream fin_stations(stations_file.c_str());
    string s;
    while (fin_stations >> s) {
      stations.push_back(s);

      vector<string> c;
      connections.insert(make_pair(s, c));

      map<string, double> f;
      fares.insert(make_pair(s, f));
    }
    fin_stations.close();

    // connections
    string connections_file = prefix + "/connections.txt";
    ifstream fin_connections(connections_file.c_str());
    string u, v;
    int d;
    while (fin_connections >> u >> d) {
      while (d > 0) {
        fin_connections >> v;
        connections[u].push_back(v);
        --d;
      }
    }
    fin_connections.close();

    // fares
    string fares_file = prefix + "/fares.txt";
    ifstream fin_fares(fares_file.c_str());
    string w, x;
    for (int i = 0; i < stations.size(); ++i) {
      fin_fares >> w;
      for (int j = i+1; j < stations.size(); ++j) {
        x = stations[j];
        double p;
        fin_fares >> p;
        fares[w].insert(make_pair(x, p));
        fares[x].insert(make_pair(w, p));
      }
    }
    fin_fares.close();
  }

  void iterate_trips() {
    for (int i = 0; i < stations.size(); ++i) {
      string u_start = stations[i];
      for (int j = i+1; j < stations.size(); ++j) {
        string u_end = stations[j];
        double u_cost = fares[u_start][u_end];

        for (int k = i+1; k < stations.size(); ++k) {
          string v_start = stations[k];
          for (int l = k+1; l < stations.size(); ++l) {
            string v_end = stations[l];
            double v_cost = fares[v_start][v_end];

            double uv_cost = u_cost + v_cost;
            double vu_cost = fares[u_start][v_end] + fares[v_start][u_end];

            if (uv_cost >= vu_cost + EPS && has_path_overlap(u_start, u_end, v_start, v_end)) {
              cout << "[" << u_start << " => " << u_end << "] + [" << v_start << " => " << v_end << "]: ";
              cout << uv_cost-vu_cost << " (" << 100*(uv_cost-vu_cost)/uv_cost << "%)\n";
            }
          }
        }
      }
    }
  }

  bool has_path_overlap(string u_start, string u_end, string v_start, string v_end) {
    vector<string> u_path;
    get_path(u_start, u_end, u_path);
    vector<string> v_path;
    get_path(v_start, v_end, v_path);

    set<string> s;
    for (int i = 0; i < u_path.size()-1; ++i) {
      s.insert(u_path[i] + "=>" + u_path[i+1]);
    }

    for (int j = 0; j < v_path.size()-1; ++j) {
      set<string>::iterator it = s.find(v_path[j] + "=>" + v_path[j+1]);
      if (it != s.end()) {
        return true;
      }
    }

    return false;
  }

  void get_path(string u, string v, vector<string>& path) {
    map<string, string> parent;
    parent.insert(make_pair(u, u));

    queue<string> q;
    q.push(u);

    while (q.size() > 0) {
      string w = q.front();
      q.pop();

      if (w == v) {
        break;
      }

      for (int j = 0; j < connections[w].size(); ++j) {
        string x = connections[w][j];
        map<string, string>::iterator it = parent.find(x);
        if (it == parent.end()) {
          parent.insert(make_pair(x, w));
          q.push(x);
        }
      }
    }

    stack<string> stk;
    while (v != parent[v]) {
      stk.push(v);
      v = parent[v];
    }
    stk.push(u);

    while (stk.size() > 0) {
      path.push_back(stk.top());
      stk.pop();
    }
  }
};

int main() {
  transit bart("sf");
  bart.read_data();
  bart.iterate_trips();
  return 0;
}
