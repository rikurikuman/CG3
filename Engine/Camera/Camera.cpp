#include "Camera.h"

using namespace std;

unique_ptr<Camera> Camera::_def = make_unique<Camera>();
Camera* Camera::nowCamera = _def.get();