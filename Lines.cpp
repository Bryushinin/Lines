#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "console09\Console.h"
#include "R64M.hpp"
#include <conio.h>
#include <fstream>

using namespace std;

const int RIGGED_GAME_START = 10;
const int RIGGED_GAME_MODE = 1; //0 will lead to counting average weight of present colors for unpresent ones
const double WEIGHT_OF_UNPRESENT_COLOR = 2.;
const int POINT_MULTIPLIER = 10;
const int FIRST_AMOUNT = 5;
const int NEXT_AMOUNT = 3;
const int X_START = 4;
const int Y_START = 4;

struct cell {
	int x;
	int y;
	bool present;
	COLORS color;
};

void blink(int x, int y)
{
	ColorPrint(x, y, B_L_WHITE | F_L_CYAN, "*");
	_sleep(40);
	ColorPrint(x, y, BLACK, " ");
	_sleep(40);
	ColorPrint(x, y, B_L_WHITE | F_L_CYAN, "*");
	_sleep(40);
}

int get_num_by_color(COLORS col)
{
	switch (col) {
	case B_L_MAGENTA: return 0;
	case B_L_GREEN: return 1;
	case B_D_CYAN: return 2;
	case B_D_BLUE: return 3;
	case B_L_RED: return 4;
	default: return 1;
	}
}

class Field {
	cell a[81];
	int filled;
	int color_presence[5];
	int turn_number;
	int remove_vertical();
	int remove_horizontal();
	int remove_diagonal1();
	int remove_diagonal2();
public:
	Field();
	void ball_input(int q = FIRST_AMOUNT);
	void game_over();
	void update_filled() { int s = 0; for (int i = 0; i < 81; s += a[i].present, ++i); filled = s; if (s == 81) game_over(); };
	cell get_cell(int x, int y) { return a[y * 9 + x]; }
	void set_present_and_color_pp(int x, int y, COLORS color0);
	void set_unpresent_pp(int x, int y);
	int get_filled() { return filled; }
	int remove_lines() { return remove_vertical() + remove_horizontal() + remove_diagonal1() + remove_diagonal2(); };
	bool root_exists(int xd, int yd, int x, int y);
	void turn_inc() { ++turn_number; };
	int rigged_game();
};

COLORS get_color_by_num(int n)
{
	switch (n) {
	case 0: return B_L_MAGENTA;
	case 1: return B_L_GREEN;
	case 2: return B_D_CYAN;
	case 3: return B_D_BLUE;
	case 4: return B_L_RED;
		//case 5: return B_L_YELLOW;
	default: return B_L_GREEN;
	}
}

void Field::set_present_and_color_pp(int x, int y, COLORS color0)
{
	int t = y * 9 + x;
	++color_presence[get_num_by_color(color0)];
	a[t].color = color0;
	a[t].present = true;
	ColorPrint(x, y, color0, " ");
}

void Field::set_unpresent_pp(int x, int y)
{
	int t = y * 9 + x;
	--color_presence[get_num_by_color(a[t].color)];
	a[t].color = BLACK;
	a[t].present = false;
	blink(x, y);
	ColorPrint(x, y, BLACK, " ");
}

void Field::ball_input(int q)
{
	int free = 81 - filled;
	for (int i = 0; i < q; ++i)
	{
		int pos = int(rnunif() * (free - 1)), j, k = 0;
		for (j = 0; k < pos; ++j)
			if (!a[j].present)
				++k;
		while (a[j].present)
			if (j == 80)
				exit(80);
			else
				++j;
		pos = j;
		int col = int(rnunif() * 5);
		if (turn_number < RIGGED_GAME_START)
			col = int(rnunif() * 5);
		else
			col = rigged_game();///////////////
		if (!a[pos].present)
		{
			set_present_and_color_pp(a[pos].x, a[pos].y, get_color_by_num(col));

			--free;
		}
		else ColorPrint(a[pos].x, a[pos].y, B_L_WHITE | F_L_RED, "%i", k);
		update_filled();
		free = 81 - filled;
	}
	update_filled();
}

Field::Field()
{
	filled = 0; turn_number = 0;
	for (int i = 0; i < 5; ++i)
		color_presence[i] = 0;
	for (int i = 0; i < 81; ++i)
	{
		a[i].present = false;
		a[i].x = i % 9;
		a[i].y = i / 9;
		a[i].color = BLACK;
	}
	ball_input();
	update_filled();
}

int Field::remove_vertical()
{
	int ans = 0;
	for (int j = 0; j < 9; ++j)
	{
		for (int start = 0; start < 37; start += 9)
		{
			if (a[start + j].present)
			{
				COLORS col = a[start + j].color;
				int found = 0;
				for (int i = start + j; i < 81; i += 9)
					if (a[i].present && (a[i].color == col))
						++found;
					else break;
				if (found >= 5)
				{
					for (int i = start + j; i < 81; i += 9)
						if (a[i].present && (a[i].color == col))
							set_unpresent_pp(a[i].x, a[i].y);
						else break;
					ans += found;
				}
			}
		}
	}
	return ans;
}

int Field::remove_horizontal()
{
	int ans = 0;
	for (int start = 0; start < 73; start += 9)
	{
		for (int j = 0; j < 5; ++j)
		{
			if (a[start + j].present)
			{
				COLORS col = a[start + j].color;
				int found = 0;
				for (int i = start + j; i < start + 9; ++i)
					if (a[i].present && (a[i].color == col))
						++found;
					else break;
				if (found >= 5)
				{
					for (int i = start + j; i < start + 9; ++i)
						if (a[i].present && (a[i].color == col))
							set_unpresent_pp(a[i].x, a[i].y);
						else break;
					ans += found;
				}
			}
		}
	}
	return ans;
}

int Field::remove_diagonal1()
{
	int ans = 0;
	int mas[9] = { 0,1,2,3,4,9,18,27,36 };
	int mas2[9] = { 80,71,62,53,44,79,78,77,76 };
	for (int start = 0; start < 9; ++start)
	{
		for (int j = 0; j < 5; ++j)
		{
			if (a[mas[start] + 10 * j].present)
			{
				COLORS col = a[mas[start] + 10 * j].color;
				int found = 0;
				for (int i = mas[start] + 10 * j; i <= mas2[start]; i += 10)
				{
					if (a[i].present && (a[i].color == col))
						++found;
					else break;
				}
				if (found >= 5)
				{
					for (int i = mas[start] + 10 * j; i <= mas2[start]; i += 10)
						if (a[i].present && (a[i].color == col))
							set_unpresent_pp(a[i].x, a[i].y);
						else break;
					ans += found;
				}
			}
		}
	}
	return ans;
}

int Field::remove_diagonal2()
{
	int ans = 0;
	int mas[9] = { 4,5,6,7,8,17,26,35,44 };
	int mas2[9] = { 36,45,54,63,72,73,74,75,76 };
	for (int start = 0; start < 9; ++start)
	{
		for (int j = 0; j < 5; ++j)
		{
			if (a[mas[start] + 8 * j].present)
			{
				COLORS col = a[mas[start] + 8 * j].color;
				int found = 0;
				for (int i = mas[start] + 8 * j; i <= mas2[start]; i += 8)
					if (a[i].present && (a[i].color == col))
						++found;
					else break;
				if (found >= 5)
				{
					for (int i = mas[start] + 8 * j; i <= mas2[start]; i += 8)
						if (a[i].present && (a[i].color == col))
							set_unpresent_pp(a[i].x, a[i].y);
						else break;
					ans += found;
				}
			}
		}
	}
	return ans;
}

void Field::game_over()
{
	ColorPrint(0, 4, B_L_WHITE | F_L_RED, "G");
	ColorPrint(1, 4, B_L_WHITE | F_L_RED, "A");
	ColorPrint(2, 4, B_L_WHITE | F_L_RED, "M");
	ColorPrint(3, 4, B_L_WHITE | F_L_RED, "E");
	ColorPrint(4, 4, B_L_WHITE, " ");
	ColorPrint(5, 4, B_L_WHITE | F_L_RED, "O");
	ColorPrint(6, 4, B_L_WHITE | F_L_RED, "V");
	ColorPrint(7, 4, B_L_WHITE | F_L_RED, "E");
	ColorPrint(8, 4, B_L_WHITE | F_L_RED, "R");
	_sleep(10000);
	exit(100);
}

void print_points(int x, int y, int points, int highest_points)
{
	if (points < highest_points)
		ColorPrint(x, y, B_D_RED | F_L_WHITE, "%i", points);
	else
	{
		ColorPrint(x, y, B_L_GREEN | F_L_WHITE, "%i", points);
		FILE* out = fopen("highest_points.txt", "w");
		fwrite(&points, sizeof(int), 1, out);
		fclose(out);
	}
}

int Field::rigged_game()
{
	double c[5], s = 0.; int k = 0;
	for (int i = 0; i < 5; ++i)
	{
		if (color_presence[i])
		{
			c[i] = 1. / color_presence[i];
			s += c[i];
			++k;
		}
		else
			if (RIGGED_GAME_MODE)
				c[i] = WEIGHT_OF_UNPRESENT_COLOR;
	}
	if (!RIGGED_GAME_MODE)
	{
		double avg = s / k;
		for (int i = 0; i < 5; ++i)
		{
			if (!color_presence[i])
			{
				c[i] = avg;
				s += c[i];
			}
		}
	}
	double val = rnunif() * s;
	if (val < c[0])
		return 0;
	double chs = c[0];
	for (int i = 0; i < 4; ++i)
	{
		if (chs < val && val < (chs += c[i + 1]))
			return i + 1;
	}
	cerr << "Failure in rigged game";
	return 5;
}

bool Field::root_exists(int xf, int yf, int x, int y)
{
	bool b[81]{}; int mas[162]{};
	for (int i = 0; i < 81; ++i)
	{
		b[i] = false;
		mas[i] = 0;
		mas[161 - i] = 0;
	}
	b[yf * 9 + xf] = true;
	mas[0] = yf * 9 + xf; int size = 1, dest = y * 9 + x;
	while (size)
	{
		int k = 0;
		for (int i = 0; i < size; ++i)
		{

			if (mas[i] > 8)
				if (!b[mas[i] - 9] && !a[mas[i] - 9].present)
				{
					if (mas[i] - 9 == dest)
						return true;
					b[mas[i] - 9] = true;
					mas[size + k++] = mas[i] - 9;
				}
			if (mas[i] < 72)
				if (!b[mas[i] + 9] && !a[mas[i] + 9].present)
				{
					if (mas[i] + 9 == dest)
						return true;
					b[mas[i] + 9] = true;
					mas[size + k++] = mas[i] + 9;
				}
			if (mas[i] % 9)
				if (!b[mas[i] - 1] && !a[mas[i] - 1].present)
				{
					if (mas[i] - 1 == dest)
						return true;
					b[mas[i] - 1] = true;
					mas[size + k++] = mas[i] - 1;
				}
			if (mas[i] % 9 != 8)
				if (!b[mas[i] + 1] && !a[mas[i] + 1].present)
				{
					if (mas[i] + 1 == dest)
						return true;
					b[mas[i] + 1] = true;
					mas[size + k++] = mas[i] + 1;
				}
		}
		for (int i = 0; i < k; ++i)
			mas[i] = mas[i + size];
		size = k;
	}
	return b[y * 9 + x];
}

void root_unavailable()
{
	ColorPrint(12, 2, B_L_GREEN | F_D_RED, "Root unavailable! =(");
	_sleep(180);
	ColorPrint(12, 2, B_L_YELLOW | F_D_RED, "Root unavailable! =(");
	_sleep(180);
	ColorPrint(12, 2, B_L_GREEN | F_D_RED, "Root unavailable! =(");
	_sleep(180);
	ColorPrint(12, 2, B_L_YELLOW | F_D_RED, "Root unavailable! =(");
	_sleep(180);
	ColorPrint(12, 2, BLACK, "Root unavailable! =(");
}

int main()
{
	FILE* in = fopen("highest_points.txt", "r");
	int highest_points; fread(&highest_points, sizeof(int), 1, in);
	fclose(in);
	InitConsole("Lines", 85, 15);
	for (int i = 0; i < 10; ++i)
	{
		ColorPrint(9, i, B_D_WHITE, " ");
		ColorPrint(i, 9, B_D_WHITE, " ");
	}
	VisibleCursor(false);
	ColorPrint(30, 1, B_D_RED | F_L_WHITE, "Points: ");
	ColorPrint(22, 5, B_D_RED | F_L_WHITE, "Highest points: ");
	ColorPrint(35, 7, B_D_RED | F_L_WHITE, "%i", highest_points);
	print_points(35, 3, 0, highest_points);
	ColorPrint(22, 9, B_D_RED | F_L_WHITE, "Controls: ");
	ColorPrint(22, 10, B_D_RED | F_L_WHITE, "q - grab/release  w - go up ");
	ColorPrint(22, 11, B_D_RED | F_L_WHITE, "a - go left       s - go down      d - go right ");
	ColorPrint(22, 12, B_D_RED | F_L_WHITE, "] - exit ");
	ColorPrint(16, 14, B_L_GREEN | F_L_WHITE, "Be careful: use only english keybord layout!");
	int x = X_START, y = Y_START, xf, yf, points = 0; bool fixed = false; cell fixed_y;
	rninit(time(0));
	Field P;



	while (1) {
		int xp = x, yp = y;
		char c = getch();
		cell t = P.get_cell(x, y);
		if (c == 'a')
			x = max((x - 1), 0);
		if (c == 'w')
			y = max((y - 1), 0);
		if (c == 'd')
			x = min((x + 1), 8);
		if (c == 's')
			y = min((y + 1), 8);
		if (c == 'q' && !fixed && t.present)
		{
			fixed = true;
			xf = x;
			yf = y;
			fixed_y = P.get_cell(x, y);
		}
		if (c == 'q' && fixed && !t.present && P.root_exists(xf, yf, x, y))
		{
			P.turn_inc();
			fixed = false;
			P.set_unpresent_pp(xf, yf);
			P.set_present_and_color_pp(x, y, fixed_y.color);

			int curr_del = P.remove_lines(); P.update_filled();
			if (curr_del)
			{
				points += POINT_MULTIPLIER * curr_del;
				print_points(35, 3, points, highest_points);
			}
			else
			{
				P.ball_input(min(81 - P.get_filled(), NEXT_AMOUNT));
				P.remove_lines();
				P.update_filled();
			}
		}
		else if (c == 'q' && fixed && !t.present)
		{
			root_unavailable();
			fixed = false;
			ColorPrint(xf, yf, fixed_y.color, " ");
			P.ball_input(min(81 - P.get_filled(), NEXT_AMOUNT));
			P.remove_lines();
			P.update_filled();
		}
		if (t.present && (xp != x || yp != y))
			ColorPrint(xp, yp, t.color, " ");
		else if (xp != x || yp != y)
			ColorPrint(xp, yp, BLACK, " ");
		ColorPrint(x, y, B_L_WHITE, " ");
		if (fixed)
			ColorPrint(xf, yf, fixed_y.color | F_L_CYAN, "X");
		if (c == ']')
			exit(911);
	}
}
