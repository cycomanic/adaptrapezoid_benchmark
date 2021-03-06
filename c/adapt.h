#ifndef ADAPTRAPEZOID_H
#define ADAPTRAPEZOID_H

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#define STACK_CAP 1024

typedef struct
{
    double x;
    double f;
} Point;

inline Point midpoint (double (*func) (double), Point p1, Point p2)
{
    double xm = (p1.x + p2.x) / 2.0;
    Point p = { .x = xm, .f = func (xm) };
    return p;
}
///////////////stack/////////////////
typedef struct _PointStack
{
    Point *mem;
    int cap;
    int top_idx; // point to the current top
} PointStack;

inline void extend_stack (PointStack *ps, int new_cap)
{
    ps->mem = (Point *)realloc (ps->mem, new_cap * sizeof (Point));
    assert (ps->mem != 0);
    ps->cap = new_cap;
    // printf("extended\n");
}

inline void push (PointStack *ps, Point p)
{
    if (ps->top_idx >= ps->cap - 1)
        {
            extend_stack (ps, ps->cap * 2);
        }
    ++ps->top_idx;
    ps->mem[ps->top_idx] = p;
}

inline Point pop (PointStack *ps)
{
    return ps->mem[ps->top_idx--];
}

inline Point top (PointStack *ps)
{
    return ps->mem[ps->top_idx];
}

inline int stack_size (PointStack *ps)
{
    return ps->top_idx + 1;
}

inline PointStack init_point_stack (int cap)
{
    Point *mem = (Point *)malloc (sizeof (Point) * cap);
    PointStack pp = { mem, cap, -1 };
    return pp;
}

inline void finalize_point_stack (PointStack *ps)
{
    free (ps->mem);
    ps->mem = 0;
    ps->cap = 0;
    ps->top_idx = -1;
}

inline void neumaier_sum(double x, double* sum, double* comp)
{
    //https://en.wikipedia.org/wiki/Kahan_summation_algorithm
    double t=*sum+x;
    if (fabs(*sum)>=fabs(x)){
        *comp+=((*sum-t)+x);
    }else{
        *comp+=((x-t)+ *sum);
    }
    *sum=t;
}



double integrate (double (*func) (double), double *init_ticks, int nticks, double eps1)
{
    if (nticks <= 1)
        {
            return 0;
        }
    double full_width = init_ticks[nticks - 1] - init_ticks[0];
    double area=0;
    double comp=0;
    PointStack ps = init_point_stack (STACK_CAP);
    for (int i = 0; i < nticks; ++i)
        {
            Point p = { .x = init_ticks[i], .f = func (init_ticks[i]) };
            push (&ps, p);
        }
    Point right=pop(&ps);
    double eps = eps1 * 4.0 / full_width;
    while (stack_size (&ps) > 0)
        {
            Point left = top (&ps);
            Point midp = midpoint (func, left, right);
            if (fabs (left.f + right.f - midp.f * 2) <= eps)
                {
                    double s = (left.f + right.f + midp.f * 2.0) * (right.x - left.x) / 4.0;
                    neumaier_sum(s, &area, &comp);
                    right=pop(&ps);
                }
            else
                {
                    push (&ps, midp);
                }
        }
    finalize_point_stack (&ps);
    return area;
}

#endif