#shader compute
#version 430 core

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;
layout(binding = 1,std430) buffer Buffer1 { int CurrentState[]; };
layout(binding = 2, std430) buffer Buffer2 { int Output[]; };

#define More 0
#define Von Neumann 1

uniform int Stride;
uniform int TotalStates;
uniform int NeighborCountMode;
uniform int AutomataIndex;

const ivec3 UP = ivec3(0.0f, 1.0f, 0.0f);
const ivec3 DOWN = -UP;
const ivec3 RIGHT = ivec3(1.0f, 0.0f, 0.0f);
const ivec3 LEFT = -RIGHT;
const ivec3 FRONT = ivec3(0.0f, 0.0f, 1.0f);
const ivec3 BACK = -FRONT;
void UpdateState(in ivec3 pos, in int id);
int Cell(in ivec3 pos);
int CalcNeighbors(in ivec3 pos);
bool NeedToRevive(int activeN);
bool NeedToDie(int activeN);

void main()
{

	ivec3 pos = ivec3(gl_GlobalInvocationID);
	bool notLowerB = pos.x > 0 && pos.y > 0 && pos.z > 0;
	int upperLim = Stride - 1;
	bool notUpperB = pos.x < upperLim&& pos.y < upperLim&& pos.z < upperLim;

	//borders are ignored
	if (notLowerB && notUpperB) {
		int index = pos.x + pos.y * Stride + pos.z * Stride * Stride;
		//Make sure the borders dont get Updated, they must remind as dead
		UpdateState(pos, index);
	}

};

void UpdateState(in ivec3 pos, in int id) {
	int cState = CurrentState[id];
	int activeN = CalcNeighbors(pos);
	if (cState == 0) {
		//dead
		if (NeedToRevive(activeN)) {
			//Born
			CurrentState[id] = TotalStates - 1;
		}
	}
	else {
		if (cState == TotalStates - 1) {
			//Alive
				//Check Survival Rules
			if (NeedToDie(activeN)) {

				CurrentState[id]--;
			}
		}
		else {
			//Check if we can use inout to pass that specific entry our buffer
			CurrentState[id]--;
		}
	}

}
int Cell(in ivec3 pos) {
	int cState = Output[pos.x + pos.y * Stride + pos.z * Stride * Stride];
	int val = 0;
	if (cState == (TotalStates - 1)) {
		val = 1;
	}
	return val;
}
int CalcNeighbors(in ivec3 pos) {
	int res = 0;
	if (NeighborCountMode == More) {
		res = Cell(pos + ivec3(-1,  1,  1))   + Cell(pos + ivec3(0,  1,  1))   + Cell(pos + ivec3(1,  1,  1))  +
			  Cell(pos + ivec3(-1,  0,  1))   + Cell(pos + ivec3(0,  0,  1))   + Cell(pos + ivec3(1,  0,  1))  +
			  Cell(pos + ivec3(-1, -1,  1))   + Cell(pos + ivec3(0, -1,  1))   + Cell(pos + ivec3(1, -1,  1))  +
			  Cell(pos + ivec3(-1,  1,  0))   + Cell(pos + ivec3(0,  1,  0))   + Cell(pos + ivec3(1,  1,  0))  +
			  Cell(pos + ivec3(-1,  0,  0))                                    + Cell(pos + ivec3(1,  0,  0))  +
			  Cell(pos + ivec3(-1, -1,  0))   + Cell(pos + ivec3(0, -1,  0))   + Cell(pos + ivec3(1, -1,  0))  +
			  Cell(pos + ivec3(-1,  1, -1))   + Cell(pos + ivec3(0,  1, -1))   + Cell(pos + ivec3(1,  1, -1))  +
			  Cell(pos + ivec3(-1,  0, -1))   + Cell(pos + ivec3(0,  0, -1))   + Cell(pos + ivec3(1,  0, -1))  +
			  Cell(pos + ivec3(-1, -1, -1))   + Cell(pos + ivec3(0, -1, -1))   + Cell(pos + ivec3(1, -1, -1));

			
	}
	else {
		res = Cell(pos + UP) + Cell(pos + DOWN) + Cell(pos + RIGHT) + Cell(pos + LEFT) + Cell(pos + FRONT) + Cell(pos + BACK);
	}
	return res;
}
bool NeedToRevive(int activeN) {
	bool needToRevive = false;
	if (AutomataIndex == 0) {
		needToRevive = (activeN > 0) && (activeN < 4);
	}
	else if (AutomataIndex == 1) {
		needToRevive = activeN == 4;
	}
	else if (AutomataIndex == 2) {
		needToRevive= (activeN > 3) && (activeN < 8);
	}
	else {
		needToRevive = (activeN > 2 && activeN < 6) || (activeN > 9 && activeN < 14);
	}

	return needToRevive;
}
bool NeedToDie(int activeN) {
	bool survives = false;
	if (AutomataIndex == 0) {
		survives=(activeN == 1) || (activeN == 4) || (activeN == 5);
	}
	else if (AutomataIndex == 1) {
		survives= activeN == 4;
	}
	else if (AutomataIndex == 2) {
		survives= (activeN > 5) && (activeN < 9);
	}
	else {
		survives= (activeN == 3);
	}
	return !survives;
}