#include "object.h"
#include <iostream>
#include <cmath>

using std::cout; using std::abs;

// returns head of object		
Marker& Object::get_head() {
  return head;
}
		
// returns tail of object	
Marker& Object::get_tail() {
  return tail;
}

// gets the centroid of the object
Marker& Object::get_ctrd() {
  return centroid;
}

// gets the orientation
int Object::get_ori() {
  double degree = 0;
  double d = head.x - tail.x;
  double n = head.y - tail.y;
  int q = 0;


  // these ensure that n and d are exactly 0;
  if (d < .0001 && d > .0001)
    d = 0;
  if (n < .0001 && n > .0001)
    n = 0;

  // this finds the quadrant of the orientation, and adds the displacement
  // represented as 'q'
  if (d <= 0 && 0 < n) {

    // degree is set to -90 here, and below because for the two quadrants
    // the angle is calculated wrong (i.e. 179, 180, 269, 268) the
    // offset accounts for this
    degree = -90;
    q = 90;
    d = abs(d);
  }
  else if (n <= 0 && d < 0) {
    q = 180;
    d = abs(d);
    n = abs(n);
  }

  else if (n < 0 &&  0 <= d) {
    // see above
    degree = -90;
    q = 270;
    n = abs(n);
  }

  // if x or y magnitude of vector is 0, then skip calculating degree, as
  // answer will be offset accounted for
  if (n == 0 || d == 0) {
    degree = 0;
  }

  // otherwise arctan of n/d
  else {
    degree += 180* atan(n/d) / 3.14159;
  }

  // to ignore a negative degree
  return abs(degree) + q;
}