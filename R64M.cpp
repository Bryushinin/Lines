#include "R64M.hpp"

static unsigned long long int iu, iuhold, mult=663608941l;

// Генерация следующего случайного числа
double rnunif () {
    //const double flt = 5.42101086242752217e-20;
    const double flt = 0x1p-64;
    iu *= mult;
    return (flt*iu);
}

// Создание seed для последовательности случайных чисел
void rninit (unsigned long long  iufir) {
    iu = ( ( iufir%2 ) ? iufir : iufir + 1 );
    iuhold = iu;
}

// Сброс последовательности к изначальному seed
void rnrest () { rninit (iuhold); }

// Создание новой мультипликативности
void rnconst (unsigned long long int multnew) {
    int rest = multnew % 8;
    if ( (rest != 5) && (rest != 3) )
        mult = multnew - rest + 5;
    else
        mult = multnew;
}

// Получение предыдущего члена последовательности
unsigned long long int rnlast () { return ( iu ); }

// Получить seed последовательности
unsigned long long int rnfirst () { return ( iuhold ); }
