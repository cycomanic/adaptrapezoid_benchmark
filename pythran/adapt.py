import math

#pythran export capsule sinx(float64)
def sinx(x):
    return math.sin(x**2)

def neumaier_sum(x, sum, comp):
    t=sum+x
    if abs(sum)>=abs(x):
        comp+=(sum-t)+x
    else:
        comp+=(x-t)+sum
    sum=t
    return (sum, comp)



def integrate_iter(func, eps, points):
    total_area = 0.0
    comp = 0.0
    if len(points) < 2:
        return 0.0

    right = points.pop()

    while points:
        left = points[-1]
        xm = 0.5 * (left[0] + right[0])
        fm = func(xm)
        if abs(left[1] + right[1] - 2 * fm) <= eps:
            area = (left[1] + right[1] + fm * 2) * (right[0] - left[0]) /4
            total_area, comp = neumaier_sum(area, total_area, comp)

            right=points.pop()
        else:
            points.append((xm, fm))
            
    return total_area+comp

#pythran export integrate( float64(float64),(float64, float64, float64, float64), float64)
def integrate(func, ticks, eps):
    eps1 = eps * 4 / (ticks[-1] - ticks[0])
    points = [(x, func(x)) for x in ticks]
    return integrate_iter(func, eps1, points)
