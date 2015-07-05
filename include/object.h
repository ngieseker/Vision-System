#ifndef OBJECT_H
#define OBJECT_H

struct Marker {
  double x;
  double y;
  double r;
  double theta;
  unsigned id;
  unsigned color;
	
  Marker() : x(-1), y(-1), color(-1) {}
  Marker(int x, int y, int c) :	x(x), y(y), color(c) {}
  
  bool operator<(const Marker &rhs) const {
    return theta < rhs.theta;
  }
};


class Object {
 public:
  
  // Constructor, takes in 4 markers and creates an object out of them
  //Object(Marker& m1, Marker& m2, Marker& m3, Marker& m4, int l);
  Object(Marker& m1, Marker& m2, Marker& m3, Marker& m4, int l) {
    
    // sets stuff for centroid
    centroid.x = (m1.x + m2.x + m3.x + m4.x)/4;
    centroid.y = (m1.y + m2.y + m3.y + m4.y)/4;
    centroid.color = 3;
    
    id = l;
    // this finds the red/head marker
    if (m1.color == 2)
      head = m1;
    else if (m2.color == 2)
      head = m2;
    else if (m3.color == 2)
      head = m3;
    else
      head = m4;
    
    // this part finds the maximum distance between the head
    // and the other markers, to determine which one is the tail
    
    // calculation for marker 1
    double x = (head.x - m1.x);
    double y = (head.y - m1.y);
    double max_dist = x*x + y*y;
    if (max_dist != 0)
      tail = m1;
    
    // marker 2
    // About how this works...
    
    // start by calculating euclidean distance
    x = (head.x - m2.x);
    y = (head.y - m2.y); 
    double dist = x*x + y*y;
			
    // if distance is less, than new tail is found
    if (max_dist < dist) {
      max_dist = dist;			
      
      // tail is now this marker
      tail = m2;
		}
    
    // markers 3 and 4 work more or less the same as marker 2
    
    // marker 3
    x = (head.x - m3.x);
    y = (head.y - m3.y); 
    dist = x*x + y*y;
    if (max_dist < dist) {
      max_dist = dist;
      tail = m3;
    }
    
    // marker 4
    x = (head.x - m4.x);
    y = (head.y - m4.y); 
    dist = x*x + y*y;
    if (max_dist < dist) {
      max_dist = dist;
      tail = m4;
    }
  }
  
  // returns head of object
  Marker& get_head();	
  
  // returns tail of object
  Marker& get_tail();
  
  // gets the centroid of the object
  Marker& get_ctrd();
  
  // gets the orientation
  int get_ori();
  
  //Might not be needed if swap approach is taken
  //merge(Object& o);
  
 private:
  int id;
  Marker head;
  Marker tail;
  //Marker o1, o2;
  Marker centroid;
  int ori;
};
#endif
