#include <bits/stdc++.h>

using namespace std;

struct Point { 
    double x; 
    double y;
    Point() {} 
    Point(double _x, double _y) { 
        x = _x; 
        y = _y; 
    } 
}; 

struct Rectangle {
    int left, right, top, bottom;
    Rectangle(){}
    Rectangle(int _left, int _right, int _top, int _bottom){
        left = _left;
        right = _right;
        top = _top;
        bottom = _bottom;
    }
};

struct Node {
    Node *parent, *child[8];
    vector<Point> points;
    Rectangle rectangle;
    Point start, end;
    double cost; //non-leaf is (cost of tsp - euclidean distance between start and end point of each leaf)
    Node(){}
    Node(Node *_parent, Rectangle _rectangle){
        parent = _parent;

        for (int i = 0; i < 8; i++)
            child[i] = NULL;
        
        rectangle = _rectangle;
        points.clear();
        cost = 0.000;        
    }
    bool isLeaf(){
        return (child[0] == NULL);
    }
};

int main(){
  
  return 0;   
}