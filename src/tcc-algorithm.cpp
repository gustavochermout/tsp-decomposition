#include <bits/stdc++.h>

using namespace std;

struct Point { 
    int x; 
    int y;
    Point() {} 
    Point(int _x, int _y) { 
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
        for (int i = 0; i < 8; i++)
            if (child[i] != NULL)
                return true;
        
        return false;
    }
};

vector<Point> instance;
int N;
#define READ_FILE false

void readInstance(char *argv[]){
    int x, y, id;
    instance.clear();

    FILE *entrada;
    entrada = fopen(argv[1], "rt");

    if (READ_FILE)
        fscanf(entrada, "%d", &N);
    else
        scanf("%d", &N);

    for (int i = 0; i < N; i++){
        if (READ_FILE)
            fscanf(entrada, "%d %d %d", &id, &x, &y);
        else
            scanf("%d %d", &x, &y);
        
        instance.push_back(Point(x, y));
    }
}

Rectangle getRectangleAroundPoints(vector<Point> points){
    Rectangle rectangle = Rectangle(INT_MAX, INT_MIN, INT_MIN, INT_MAX);

    for (int i = 0; i < points.size(); i++){
        rectangle.left = min(rectangle.left, points[i].x - 1);
        rectangle.right = max(rectangle.right, points[i].x + 1);
        rectangle.top = max(rectangle.top, points[i].y + 1);
        rectangle.bottom = min(rectangle.bottom, points[i].y - 1);
    }

    return rectangle;
}

#define K 10
#define INF 0x3f3f3f3f3f3f3fLL
double graph[K][K], memo[K][1 << K], visited[K][1 << K];

double tspSolve(int id, int bitmask, int size){
	if (__builtin_popcount(bitmask) == size)
		return graph[id][0];

    double &ans = memo[id][bitmask];
		
	if (visited[id][bitmask])
		return ans;
		
    visited[id][bitmask] = 1;
    ans = INF;    
	
	for(int i = 0; i < size; i++)
		if (!(bitmask & (1 << i)))
			ans = min(ans, graph[id][i] + tspSolve(i, (bitmask | (1 << i)), size));
			
	return ans;			
}

void buildGraph(vector<Point> points){
    memset(visited, 0, sizeof visited);
    memset(graph, 0, sizeof graph);

    for (int i = 0; i < points.size(); i++)
        for (int j = i + 1; j < points.size(); j++)
            graph[i][j] = graph[j][i] = hypot(points[i].x - points[j].x, points[i].y - points[j].y);
}

double closestPointPairsBetweenTwoSets(vector<Point> setU, vector<Point> setV){
    double distance = INF;
    
    for (int i = 0; i < setU.size(); i++)
        for (int j = 0; j < setV.size(); j++)
            distance = min(distance, hypot(setU[i].x - setV[j].x, setU[i].y - setV[j].y));
    
    return distance;
}

vector<Point> getPointsInsideRectangle(vector<Point> points, Rectangle rectangle){
    vector<Point> pointsInsideRectangle;

    for (int i = 0; i < points.size(); i++){
        if ((points[i].x > rectangle.left) && (points[i].x <= rectangle.right) && 
            (points[i].y > rectangle.bottom) && (points[i].y <= rectangle.top))
            pointsInsideRectangle.push_back(points[i]);
    }

    return pointsInsideRectangle;
}

int getMiddleSegment(int start, int end){
    return ((start + end) / 2);
}

Rectangle getTopRectangle(Rectangle rectangle, int left, int right){
    return Rectangle(left, right, rectangle.top, getMiddleSegment(rectangle.top, rectangle.bottom));
}

Rectangle getDownRectangle(Rectangle rectangle, int left, int right){
    return Rectangle(left, right, getMiddleSegment(rectangle.top, rectangle.bottom) + 1, rectangle.bottom);
}

pair<int, int> getFirstRectangle(Rectangle rectangle, int middleHorizontal){
    return pair<int, int>(rectangle.left, getMiddleSegment(rectangle.left, middleHorizontal));
}

pair<int, int> getSecondRectangle(Rectangle rectangle, int middleHorizontal){
    return pair<int, int>(getMiddleSegment(rectangle.left, middleHorizontal) + 1, middleHorizontal);
}

pair<int, int> getThirdRectangle(Rectangle rectangle, int middleHorizontal){
    return pair<int, int>(middleHorizontal + 1, getMiddleSegment(middleHorizontal, rectangle.right));
}

pair<int, int> getFourthRectangle(Rectangle rectangle, int middleHorizontal){
    return pair<int, int>(getMiddleSegment(middleHorizontal, rectangle.right) + 1, rectangle.right);
}

vector<Rectangle> getChildRectangles(Rectangle rectangle){
    int middleHorizontal = getMiddleSegment(rectangle.left, rectangle.right); 
    pair<int, int> firstRectangle = getFirstRectangle(rectangle, middleHorizontal);
    pair<int, int> secondRectangle = getSecondRectangle(rectangle, middleHorizontal);
    pair<int, int> thirdRectangle = getThirdRectangle(rectangle, middleHorizontal);
    pair<int, int> fourthRectangle = getFourthRectangle(rectangle, middleHorizontal);

    vector<Rectangle> childRectangles;
    childRectangles.push_back(getTopRectangle(rectangle, firstRectangle.first, firstRectangle.second));
    childRectangles.push_back(getTopRectangle(rectangle, secondRectangle.first, secondRectangle.second));
    childRectangles.push_back(getTopRectangle(rectangle, thirdRectangle.first, thirdRectangle.second));
    childRectangles.push_back(getTopRectangle(rectangle, fourthRectangle.first, fourthRectangle.second));
    
    childRectangles.push_back(getDownRectangle(rectangle, firstRectangle.first, firstRectangle.second));
    childRectangles.push_back(getDownRectangle(rectangle, secondRectangle.first, secondRectangle.second));
    childRectangles.push_back(getDownRectangle(rectangle, thirdRectangle.first, thirdRectangle.second));
    childRectangles.push_back(getDownRectangle(rectangle, fourthRectangle.first, fourthRectangle.second));

    return childRectangles;
}

void buildTree(Node *node){
    
}

int main(int argc, char *argv[]){
    readInstance(argv);
    Node node = Node(NULL, getRectangleAroundPoints(instance));
    buildTree(&node);

    return 0;   
}