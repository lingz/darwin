#include <iostream>
#include <numeric>
#include <cmath>
#include <cstdlib>
#include <cassert>

#include "edmondskarp.h"

bool isProblemA;
vector< vector< bool > > canonical;
vector< int > R, C;
int rows, columns;

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

void my_printm(vector< vector< bool > > M){
  for ( int i = 0; i < M.size(); i++ ){
    for ( int j = 0; j < M[i].size(); j++ ){
      cout << M[i][j] << " ";
    }
    cout << endl;
  }
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

int dotprod(vector< bool > v1, vector< bool > v2){
  int pr = 0;
  for ( int i = 0; i < v1.size(); i++ ){
    pr += v1[i] && v2[i];
  }
  return pr;
}

vector< vector< vector< bool > > > matrices[(int) (25+1e6)];

vector< vector< int > > S;
int score;

void flip( int r1, int r2, int c1, int c2){
  for ( int i = 0; i < rows; i++ ){
    score -= S[r1][i];
    score -= S[r2][i];
  }

  canonical[r1][c1] = !canonical[r1][c1];
  canonical[r1][c2] = !canonical[r1][c2];
  canonical[r2][c1] = !canonical[r2][c1];
  canonical[r2][c2] = !canonical[r2][c2];

  for ( int i = 0; i < rows; i++ ){
    S[r1][i] = pow(dotprod(canonical[r1], canonical[i]), 2);
    score += S[r1][i];
    S[r2][i] = pow(dotprod(canonical[r2], canonical[i]), 2);
    score += S[r2][i];
  }
}

int randflip( int r1, int r2 ){
  for ( int i = 0; i < columns; i++ ){
    for ( int j = i + 1; j < columns; j++ ){
      if ( canonical[r1][i] && !canonical[r1][j] && !canonical[r2][i] && canonical[r2][j] ){
	flip(r1, r2, i, j);
	return 1;
      }
    }
  }
  return 0;
}

int climb( int r1, int r2 ){
  assert(r1 < rows); assert(r2 < rows);
  int bc1 = -1, bc2 = -1;
  int bestscore = score;
  for ( int i = 0; i < columns; i++ ){
    for ( int j = i + 1; j < columns; j++ ){
      if ( canonical[r1][i] && !canonical[r1][j] && !canonical[r2][i] && canonical[r2][j] ){
	flip(r1, r2, i, j);
	if ( score >= bestscore ){
	  bestscore = score; 
	  bc1 = i;
	  bc2 = j;
	}
	flip(r1, r2, i, j);
      }
    }
  }
  if (bc1 >= 0 && bc2 >=0){
    flip(r1, r2, bc1, bc2);
    return 1;
  }
  return 0;
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

void makeMany(){
  srand(time(NULL));
  score = 0;
  S.resize(rows, vector< int >(rows, 0));
  for ( int i = 0; i < rows; i++ ){
    for ( int j = 0; j < rows; j++ ){
      S[i][j] = pow(dotprod(canonical[i], canonical[j]), 2);
      score += S[i][j];
    }
  }
  
  int total = 1;
  int hashc = mhash(canonical);
  matrices[hashc].push_back(canonical);
  my_print_std_out(canonical);

  int r1, r2;
  bool issame;
  while(total < 10000){

    r1 = rand() % rows;
    r2 = rand() % rows;
    if ( r1 == r2 ){
      continue;
    }
    
    randflip(r1, r2);

    hashc = mhash(canonical);

    issame = false;
    for ( int i = 0; i < matrices[hashc].size(); i++ ){
      if ( isequal(matrices[hashc][i], canonical) ){
	issame = true;
      }
    }

    if (!issame){
      matrices[hashc].push_back(canonical);
      total++;
      my_print_std_out(canonical);
    }
  }
}

void makeBest(){
  srand(time(NULL));
  score = 0;
  S.resize(rows, vector< int >(rows, 0));
  for ( int i = 0; i < rows; i++ ){
    for ( int j = 0; j < rows; j++ ){
      S[i][j] = pow(dotprod(canonical[i], canonical[j]), 2);
      score += S[i][j];
    }
  }

  int r1, r2, bestscore = score;
  vector< vector< bool > > best = canonical;
  clock_t start = clock();
  
  while( (clock() - start)/ (double) CLOCKS_PER_SEC < 110 ){
    r1 = rand() % rows;
    r2 = rand() % rows;
    if ( r1 == r2 ){
      continue;
    }
    
    int prob = rand() % 100;
    if ( prob < 50 && prob > 44 ){
      randflip(r1, r2);
    }
    else{
      climb(r1, r2);
    }
    if (score > bestscore ){
      bestscore = score;
      best = canonical;
    }
  }
  
  my_print_std_out(best);
  
}

int main(){
  get_input();
  
  makeCanonical();
  
  if (isProblemA) {
    makeMany();
  } else {
    makeBest();
  }
}
