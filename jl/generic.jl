#!/usr/bin/env julia
using Pkg
Pkg.add("BenchmarkTools")
using BenchmarkTools

struct Point{T<:AbstractFloat}
    x::T
    f::T
end

function integrate(func::Function, ticks::Array{T,1}, eps::T)::T where {T<:AbstractFloat }
    if length(ticks)<2
        return 0.0
    end
    points::Array{Point{T}, 1}=[Point(x, func(x)) for x in ticks]
    full_width=last(ticks)-first(ticks)
    areas=T[]
    right=last(points)
    while length(points)>1
        left=points[lastindex(points)-1]
        mid=(left.x+right.x)/2.0
        fmid=func(mid)
        if abs(left.f+right.f-fmid*2.0)<=eps
            area=((left.f+right.f+fmid*2.0)*(right.x-left.x)/4.0)::T
            push!(areas, area)
            pop!(points)
            right=left
        else
            pop!(points)
            push!(points, Point(mid, fmid))
            push!(points, right)
        end
    end
    sort!(areas, by=abs)
    sum(areas)
end

function integrate_nosort(func::Function, ticks::Array{T,1}, eps::T)::T where {T<:AbstractFloat }
    if length(ticks)<2
        return 0.0
    end
    points::Array{Point{T}, 1}=[Point(x, func(x)) for x in ticks]
    full_width=last(ticks)-first(ticks)
    area=0.0::T
    right=last(points)
    while length(points)>1
        left=points[lastindex(points)-1]
        mid=(left.x+right.x)/2.0
        fmid=func(mid)
        if abs(left.f+right.f-fmid*2.0)<=eps
            area+=((left.f+right.f+fmid*2.0)*(right.x-left.x)/4.0)::T
            pop!(points)
            right=left
        else
            pop!(points)
            push!(points, Point(mid, fmid))
            push!(points, right)
        end
    end
    area
end

println("Benchmarking integration with sorting sub-interval areas")
b=@benchmarkable integrate(x->sin(x^2), [0.0, 1.0, 2.0, sqrt(8*pi)], 1e-10)
tune!(b)
println(run(b))

println("Benchmarking integration without sort sub-interval areas")
b=@benchmarkable integrate_nosort(x->sin(x^2), [0.0, 1.0, 2.0, sqrt(8*pi)], 1e-10)
tune!(b)
println(run(b))
