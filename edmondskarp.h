#ifndef EDMONDSKARP_
#define EDMONDSKARP_

#include <vector>
#include <queue>

using namespace std;

class EdmondsKarp{
 private:
  vector< vector< int > > graph;
  vector< vector< int > > rGraph;
  vector< vector< int > > capacities;
  vector< int > parent;
  vector< bool > visited;
  int source, sink;
  int flow;

 public:
  EdmondsKarp(vector< vector< int > > graph, vector< vector< int > > capacities, int source, int sink ){
    this->graph = graph;
    this->capacities = capacities;
    this->rGraph = capacities;
    this->source = source;
    this->sink = sink;
    parent.resize(rGraph.size());
    visited.resize(rGraph.size());
    makeFlow();
  }

  bool bfs(){
    for ( int i = 0; i < visited.size(); i++ ){
      visited[i] = false;
    }
    queue< int > q;
    visited[source] = true;
    parent[source] = -1;
    q.push(source);
    int qf, v;
    while(!q.empty()){
      qf = q.front(); q.pop();
      for ( int i = 0; i < graph[qf].size(); i++ ){
	v = graph[qf][i];
	if ( !visited[v] && rGraph[qf][v] > 0 ){
	  q.push(v);
	  parent[v] = qf;
	  visited[v] = true;
	}
      }
    }
    return visited[sink];
  }

  int makeFlow(){
    int INF = 9+1e9;
    flow = 0;
    int u, v, path_flow;
    while(bfs()){
      path_flow = INF;
      for ( v = sink; v != source; v = parent[v] ){
	u = parent[v];
	path_flow = min(path_flow, rGraph[u][v]);
      }
      for ( v = sink; v != source; v = parent[v] ){
	u = parent[v];
	rGraph[u][v] -= path_flow;
	rGraph[v][u] += path_flow;
      }
      flow += path_flow;
    }
    return 1;
  }

  int getFlow(){
    return flow;
  }

  vector< vector< int > > getFlowGraph(){
    vector< vector< int > > flowGraph(capacities.size(), vector< int > (capacities.size(), 0));
    for ( int i = 0; i < capacities.size(); i++ ){
      for ( int j = 0; j < capacities[i].size(); j++ ){
	if ( capacities[i][j] > 0 ){
	  flowGraph[i][j] = capacities[i][j] - rGraph[i][j];
	}
      }
    }
    return flowGraph;
  }
};

#endif
