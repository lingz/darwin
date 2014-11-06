#include <iostream>
#include <numeric>
#include <algorithm>
#include <set>
#include <cmath>
#include <cassert>
#include <ctime>

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
    if ( s ){
      h = (h * s) % MOD;
    }
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

int dotprod(vector< bool > v1, vector< bool > v2){
  int pr = 0;
  for ( int i = 0; i < v1.size(); i++ ){
    pr += v1[i] && v2[i];
  }
  return pr;
}

// computes the current score
int score() {
  int score = 0;
  for ( int i = 0; i < rows; i++ ){
    for ( int j = 0; j < rows; j++ ){
      score += pow(dotprod(canonical[i],canonical[j]), 2);
    }
  }
  return score;
}

void flip(OnBit &a, OnBit &b) {

  canonical[a.row][a.column] = false;
  canonical[b.row][b.column] = false;


  // swap a and b
  int temp = a.row;
  a.row = b.row;
  b.row = temp;


  canonical[a.row][a.column] = true;
  canonical[b.row][b.column] = true;

}

vector<OnBit> buildOnBits() {
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
  return onBits;
}

bool isequal(vector< vector< bool > > m1, vector< vector< bool > > m2){
  for ( int i = 0; i < m1.size(); i++ ){
    for ( int j = 0; j < m1[i].size(); j++ ){
      if ( m1[i][j] != m2[i][j] ){
	return false;
      }
    }
  }
  return true;
}


vector< vector< vector< bool > > > matrices[(int) (25+1e6)];
void iterate() {
  vector<OnBit> onBits;
  int uniqueIterations = 0;
  int lastScore = 0;
  vector<vector< bool> > best;
  bool is_in = false;
  bool mustMutate = false;
  int newScore;
  int newHash;
  srand(time(NULL));
  clock_t start = clock();
  while ((clock() - start)/ (double) CLOCKS_PER_SEC < 110 ) {
    onBits = buildOnBits();
    random_shuffle(onBits.begin(), onBits.end());

    // 10% chance to allow going lower
    bool canGoLower = rand() % 100 < 3;
    if (mustMutate) {
      canGoLower = true;
      mustMutate = false;
    }

    bool found = false;

    for (int i = 0; i < totalPoints; i++) {
      if (found) break;
      OnBit a = onBits[i];
      for (int j = 0; j < totalPoints; j++) {
        if (j == i) continue;
        OnBit b = onBits[j];

        // check if diagonals are free
        if (!canonical[a.row][b.column] && !canonical[b.row][a.column]) {
          flip(a, b);
          
          // test
          if (isProblemA) {
            is_in = false;
	    newHash = mhash(canonical);
            for ( int i = 0; i < matrices[newHash].size(); i++ ){
              if ( isequal(matrices[newHash][i], canonical) ){
	        is_in = true;
              }
            }
          } else {
            newScore = score();
          }

          if (isProblemA) {
	    if (is_in) {
              flip(a,b);
              continue;
            }
            // stuff specific to problem A
            matrices[newHash].push_back(canonical);
            uniqueIterations++;

            my_print_std_out(canonical);
            if (uniqueIterations > 10000) {
              // end function
              return;
            }
            // done with current transition
            found = true;
            break;
	  } else if (canGoLower || newScore > lastScore) {
            if (newScore > lastScore)  {
              best = canonical;
            }
            lastScore = newScore;
            found = true;
            break;
          } else {
            flip(a,b);
          }

        }
      }
    }

    // at end, break out if we didn't find anything
    if (!found) {
	    if (!isProblemA) {
	      mustMutate = true;
	    } else {
	      cout << "Broken" << endl;
	      break;
	    }
    }
  }

  if (!isProblemA) {
    // print output type B here
    my_print_std_out(best);
  }
}


void my_print_std_out(vector<vector< bool > > M) {
  bool isFirst = true;
  for ( int i = 0; i < rows; i++ ){
    for ( int j = 0; j < columns; j++ ){
      if (!isFirst) {
        cout << " ";
      } else {
        isFirst = false;
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

int main(){
  get_input();
  makeCanonical();
  iterate();
}
