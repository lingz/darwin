#include <iostream>
#include <numeric>
#include <algorithm>
#include <unordered_set>

#include "edmondskarp.h"

bool isProblemA;
vector< vector< bool > > canonical;
vector< int > R, C;
int rows, columns;
int totalPoints;

void my_print_std_out(vector<vector< bool > > M);

int get_input(){
  cin >> isProblemA;
  cin >> rows >> columns;
  R.resize(rows); C.resize(columns);
  for ( int i = 0; i < rows; i++ ){
    cin >> R[i];
  }
  for ( int i = 0; i < columns; i++ ){
    cin >> C[i];
  }
  totalPoints = accumulate(C.begin(), C.end(), 0);
  canonical.resize(rows, vector< bool >(columns, false));
}

int makeCanonical(){
  int tot = 2 + rows + columns;
  vector< vector< int > > graph(tot), capacities(tot, vector<int>(tot, 0));
  graph.clear();
  graph.resize(tot);
  for ( int i = 1; i < rows + 1; i++ ){
    graph[0].push_back(i);
    graph[i].push_back(0);
    capacities[0][i] = R[i-1];
  }
  for ( int i = 1; i < rows + 1; i++ ){
    for ( int j = rows + 1; j < rows + columns + 1; j++ ){
      graph[i].push_back(j);
      graph[j].push_back(i);
      capacities[i][j] = 1;
    }
  }
  for ( int i = rows + 1; i < rows + columns + 1; i++ ){
    graph[i].push_back(rows + columns + 1);
    graph[rows + columns + 1].push_back(i);
    capacities[i][rows + columns + 1] = C[i-rows-1];
  }

  EdmondsKarp edk(graph, capacities, 0, tot-1);

  vector< vector< int > > flo = edk.getFlowGraph();
  for ( int i = 1; i < rows + 1; i++ ){
    for ( int j = rows + 1; j < rows + columns + 1; j++ ){
      if ( flo[i][j] == 1 ){
	canonical[i-1][j-rows-1] = true;
      }
    }
  }

  return 1;
}

int mhash(vector< vector< bool > > mat){
  int MOD = 19+1e6;
  long long int h = 1;
  int s = 0;
  for ( int i = 0; i < rows; i++ ){
    s = 0;
    for ( int j = 0; i + j < rows && j < columns; j++ ){
      s += mat[i + j][j];
    }
    h = (h * s) % MOD;
  }
  return h;
}

struct matrixHash {
  size_t operator () (const vector<vector<bool > > &x) const {return mhash(x);}
};


struct OnBit {
  int row;
  int column;
};

// computes the current score
int score() {
  return 1;
}

void iterate() {

  vector<OnBit> onBits;
  onBits.resize(totalPoints);

  int current = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (canonical[i][j]) {
        onBits[current].row = i;
        onBits[current].column = j;
        current++;
      }
    }
  }

  int uniqueIterations = 0;
  int lastScore;
  int steps;
  int MAX_STEPS = 100000;
  vector<vector< bool> > best;
  unordered_set< vector < vector< bool > >, matrixHash> seenMatricies;
  bool is_in = false;
  while (true) {
    random_shuffle(onBits.begin(), onBits.end());
    steps++;
    if (steps > MAX_STEPS) {
      break;
    }

    // 10% chance to allow going lower
    bool canGoLower = rand() % 100 < 10;

    for (int i = 0; i < totalPoints; i++) {
      OnBit a = onBits[i];
      for (int j = 0; j < totalPoints; j++) {
        OnBit b = onBits[j];
        // check if diagonals are free
        if (!canonical[a.row][b.column] && !canonical[b.row][a.column]) {
          // swap a and b
          int temp = a.row;
          a.row = b.row;
          b.row = temp;
          
          // test
          int newScore = score();
          if (isProblemA) {
            is_in = seenMatricies.find(canonical) != seenMatricies.end();
          }
          if (canGoLower || newScore > lastScore &&
              (!isProblemA || !is_in)) {
            lastScore = newScore;
            best = canonical;


            // stuff specific to problem A
            if (isProblemA) {
              seenMatricies.insert(canonical);
              uniqueIterations++;
              // print output type A here
              my_print_std_out(canonical);
              if (uniqueIterations > 10000) {
                // end function
                return;
              }
            }
            // done with current transition
            break;
          } else {
            // roll back
            temp = a.row;
            a.row = b.row;
            b.row = temp;
          }

        }
      }
    }

    // at end, break out
    break;
  }

  if (!isProblemA) {
    // print output type B here
    my_print_std_out(best);
  }
}


void my_print_std_out(vector<vector< bool > > M) {
  bool isFirst = false;
  for ( int i = 0; i < rows; i++ ){
    for ( int j = 0; j < columns; j++ ){
      if (!isFirst) {
        cout << " ";
      } else {
        isFirst = true;
      }
      cout << M[i][j];
    }
  }
  cout << endl;
}

void my_prints(vector< vector< bool > > M){
  int tot;
  int rtot = 0, ctot = 0;
  for ( int i = 0; i < rows; i++){
    rtot += accumulate( M[i].begin(), M[i].end(), 0);
    cout << accumulate( M[i].begin(), M[i].end(), 0) << " ";
  }
  cout << endl;
  for ( int i = 0; i < columns; i++ ){
    tot = 0;
    for ( int j = 0; j < rows; j++ ){
      if ( M[j][i] ){
	tot++;
      }
    }
    ctot += tot;
    cout << tot << " ";
  }
  cout << endl;
  cout << rtot << " " << ctot << endl;
}

void optimize() {
}

int main(){
  get_input();
  
  makeCanonical();
  
  my_prints(canonical);

  iterate();
  

}
