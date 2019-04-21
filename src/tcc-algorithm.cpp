#include <bits/stdc++.h>

using namespace std;

struct Point { 
    int x; 
    int y;
    Point() {
        x = 0;
        y = 0;
    } 
    Point(int _x, int _y) { 
        x = _x; 
        y = _y; 
    } 
}; 

bool operator==(Point a, Point b){
    return ((a.x == b.x) && (a.y == b.y));
}

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

vector<Point> instance;

struct Node {
    Node *parent, *child[8];
    vector<Point> points;
    Rectangle rectangle;
    Point childStart[8], childEnd[8];
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
                return false;
        
        return true;
    }
    vector<Point> getParentPoints(){
        return (parent != NULL) ? parent->points : instance;
    }
    double getCost(){
        double finalCost = cost;
        
        for (int i = 0; i < 8; i++)
            if (child[i] != NULL){
                finalCost -= hypot(child[i]->start.x - child[i]->end.x, child[i]->start.y - child[i]->end.y);
                finalCost += child[i]->getCost();
            }

        return finalCost;
    }
};

int N;
#define READ_FILE true

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

    rectangle.right = ((rectangle.right - rectangle.left) >= 4) ? rectangle.right : rectangle.left + 4;
    rectangle.top = ((rectangle.top - rectangle.bottom) >= 4) ? rectangle.top : rectangle.bottom + 4;
 
    return rectangle;
}

#define K 10
#define INF 0x3f3f3f3f3f3f3fLL
double graph[K][K], memo[K][1 << K], visited[K][1 << K];

double tspSolve(int id, int bitmask, int size, bool backToSource){
	if (__builtin_popcount(bitmask) == size)
		return (backToSource) ? graph[id][0] : 0;

    double &ans = memo[id][bitmask];
		
	if (visited[id][bitmask])
		return ans;
		
    visited[id][bitmask] = 1;
    ans = INF;    
	
	for(int i = 0; i < size; i++)
		if (!(bitmask & (1 << i)))
			ans = min(ans, graph[id][i] + tspSolve(i, (bitmask | (1 << i)), size, backToSource));
			
	return ans;			
}

void buildGraphUsingPoints(vector<Point> points){
    memset(visited, 0, sizeof visited);
    memset(graph, 0, sizeof graph);

    for (int i = 0; i < points.size(); i++)
        for (int j = i + 1; j < points.size(); j++)
            graph[i][j] = graph[j][i] = hypot(points[i].x - points[j].x, points[i].y - points[j].y);
}

double closestPointPairsBetweenTwoSets(Node *node, int u, int v){
    double distance = INF;
    
    for (int i = 0; i < node->child[u]->points.size(); i++)
        for (int j = 0; j < node->child[v]->points.size(); j++){
            double x = hypot(node->child[u]->points[i].x - node->child[v]->points[j].x, 
                node->child[u]->points[i].y - node->child[v]->points[j].y);

            if (x < distance){
                distance = x;
                node->child[u]->childEnd[v] = node->child[u]->points[i];
                node->child[v]->childStart[u] = node->child[v]->points[j];
            }
        }

    return distance;
}

void buildGraphUsingNodes(Node *node){
    memset(visited, 0, sizeof visited);
    memset(graph, 0, sizeof graph);

    for (int i = 0; i < 8; i++)
        for (int j = i + 1; j < 8; j++)
            if (node->child[i] != NULL && node->child[j] != NULL)
                graph[i][j] = graph[j][i] = closestPointPairsBetweenTwoSets(node, i, j);
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
    return Rectangle(left, right, getMiddleSegment(rectangle.top, rectangle.bottom), rectangle.bottom);
}

pair<int, int> getFirstRectangle(Rectangle rectangle, int middleHorizontal){
    return pair<int, int>(rectangle.left, getMiddleSegment(rectangle.left, middleHorizontal));
}

pair<int, int> getSecondRectangle(Rectangle rectangle, int middleHorizontal){
    return pair<int, int>(getMiddleSegment(rectangle.left, middleHorizontal), middleHorizontal);
}

pair<int, int> getThirdRectangle(Rectangle rectangle, int middleHorizontal){
    return pair<int, int>(middleHorizontal, getMiddleSegment(middleHorizontal, rectangle.right));
}

pair<int, int> getFourthRectangle(Rectangle rectangle, int middleHorizontal){
    return pair<int, int>(getMiddleSegment(middleHorizontal, rectangle.right), rectangle.right);
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

vector<Point> removeOverlappingPoints(vector<Point> points){
    for (int i = 0; i < points.size(); i++)
        for (int j = i + 1; j < points.size(); j++)
            if (points[i] == points[j]){
                points.erase(points.begin() + i), i--;
                break;
            }
        
    return points;
}

void buildTree(Node *node, int idChild){
    vector<Rectangle> rectangleChild = getChildRectangles(node->rectangle);
    node->points = getPointsInsideRectangle(node->getParentPoints(), node->rectangle);

    if (node->points.size() == 0){
        node->parent->child[idChild] = NULL;
    } 
    else if (node->points.size() > 8){
        for (int i = 0; i < 8; i++){
            node->child[i] = new Node(node, rectangleChild[i]);
            buildTree(node->child[i], i);
        }
    }
}

void showTree(Node *node, int level, int child){
    printf("---------------------\n");
    printf("Level: %d | Child: %d | Rectangle: %d %d %d %d | Leaf: %d | Cost: %.2lf \n", level, child, node->rectangle.left, node->rectangle.right, 
        node->rectangle.top, node->rectangle.bottom, node->isLeaf(), node->getCost());
    printf("Points: ");
    for (int i = 0; i < node->points.size(); i++)
        printf("(%d %d) ", node->points[i].x, node->points[i].y);
    printf("\n");

    for (int i = 0; i < 8; i++)
        if (node->child[i] != NULL)
            showTree(node->child[i], level + 1, i);
}

void buildSolution(Node *node){
    if (node->isLeaf()){
        buildGraphUsingPoints(node->points);
        node->cost = tspSolve(0, 0, node->points.size(), true);
    }else{
        for (int i = 0; i < 8; i++)
            if (node->child[i] != NULL)
                buildSolution(node->child[i]);
    }
}

int main(int argc, char *argv[]){
    readInstance(argv);
    
    if (instance.size() == 0)
        return 0;
    
    instance = removeOverlappingPoints(instance);
    Node node = Node(NULL, getRectangleAroundPoints(instance));
    buildTree(&node, 0);
    buildSolution(&node);
    showTree(&node, 1, 0);
    
    printf("%.2lf\n", node.getCost());
    
    return 0;   
}