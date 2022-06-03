#pragma once

#include "graphics/material.h"

#include <iostream>

namespace VCL {

class Object
{
public:

  const Material *mat_;

public:

  Object(const Material *const mat) : mat_(mat) { };

  virtual ~Object() = default;

  const Material *Mat() const { return mat_; }

  virtual real Intersect(const Ray &ray) const = 0;

  virtual Vec3 ClosestNormal(const Vec3 &pos) const = 0;
};

class Plane : public Object
{
protected:

  const Vec3 pos_;
  const Vec3 n_;

public:

  Plane(const Material *const mat, const Vec3 &pos, const Vec3 &dir) :
    Object(mat),
    pos_(pos),
    n_(dir.normalized())
  { }

  virtual ~Plane() = default;

  virtual real Intersect(const Ray &ray) const override
  {
    real num = (pos_ - ray.ori_).dot(n_);
    real den = ray.dir_.dot(n_);
    if (den > -EPS_) return std::numeric_limits<real>::infinity();
    return (num / den);
  }

  virtual Vec3 ClosestNormal(const Vec3 &position) const { return n_; }
};

class Sphere : public Object
{
protected:

  const Vec3 cen_;
  const real rad_;

public:

  Sphere(const Material *const mat, const Vec3 &cen, const real rad) :
    Object(mat),
    cen_(cen),
    rad_(rad)
  { }

  virtual ~Sphere() = default;

  virtual real Intersect(const Ray &ray) const override
  {
    real b = ray.dir_.dot(ray.ori_ - cen_);
    real c = (ray.ori_ - cen_).dot(ray.ori_ - cen_) - rad_ * rad_;
    real delta = b * b - c;
    if (delta < 0) return std::numeric_limits<real>::infinity();
    real t1 = -b - std::sqrt(delta), t2 = -b + std::sqrt(delta);
    if (t2 < 0) return std::numeric_limits<real>::infinity();
    else if (t1 < 0) return t2;
    return t1;
  }

  virtual Vec3 ClosestNormal(const Vec3 &pos) const { return (pos - cen_).normalized(); }
};

class Tetrahedron : public Object
{
protected:

  Vec3 p_[4];
  Vec3 n_[4];

  int idx_[4][4] = {{0, 1, 2, 3}, {3, 0, 1, 2}, {2, 3, 0, 1}, {1, 2, 3, 0}};

public:

  Tetrahedron(const Material *const mat, const Vec3 &p1, const Vec3 &p2, const Vec3 &p3, const Vec3 &p4) :
    Object(mat)
  {
    p_[0] = p1, p_[1] = p2, p_[2] = p3, p_[3] = p4;
    for (int i = 0; i < 4; ++i)
    {
      Vec3 a = p_[idx_[i][1]] - p_[idx_[i][0]];
      Vec3 b = p_[idx_[i][2]] - p_[idx_[i][0]];
      n_[i] = (a.cross(b)).normalized();
      if ((p_[idx_[i][0]] - p_[idx_[i][3]]).dot(n_[i]) < 0) n_[i] = -n_[i];
    }
  }

  virtual ~Tetrahedron() = default;

  virtual real Intersect(const Ray &ray) const override
  {
    real t = std::numeric_limits<real>::infinity();
    for (int i = 0; i < 4; ++i)
    {
      real num = (p_[i] - ray.ori_).dot(n_[i]);
      real den = ray.dir_.dot(n_[i]);
      real tmp = num / den;
      if (den < EPS_ && tmp > 0)
      {
        if (!(tmp < t)) continue;

        Vec3 P = ray.ori_ + ray.dir_ * tmp;
        Vec3 PA = p_[idx_[i][0]] - P;
        Vec3 PB = p_[idx_[i][1]] - P;
        Vec3 PC = p_[idx_[i][2]] - P;

        real z1 = PA.cross(PB).z();
        real z2 = PB.cross(PC).z();
        real z3 = PC.cross(PA).z();
        if ((z1 * z2 > 0) && (z2 * z3 > 0)) t = tmp;
      }
    }
    return t;
  }

  virtual Vec3 ClosestNormal(const Vec3 &pos) const 
  { 
    for (int i = 0; i < 4; ++i)
    {
      Vec3 PA = p_[idx_[i][0]] - pos;
      Vec3 PB = p_[idx_[i][1]] - pos;
      Vec3 PC = p_[idx_[i][2]] - pos;

      real z1 = PA.cross(PB).z();
      real z2 = PB.cross(PC).z();
      real z3 = PC.cross(PA).z();
      if ((z1 * z2 > 0) && (z2 * z3 > 0)) return n_[i];
    }
  }
};

class Cuboid : public Object
{
protected:

  const Vec3 cen_;
  const real l_;
  const real h_;
  const real w_;

  Vec3 pos_[6];
  Vec3 n_[6] = {Vec3(1, 0, 0), Vec3(-1, 0, 0), Vec3(0, 1, 0), Vec3(0, -1, 0), Vec3(0, 0, 1), Vec3(0, 0, -1)};

public:

  Cuboid(const Material *const mat, const Vec3 &cen, const real l, const real h, const real w) :
    Object(mat),
    cen_(cen),
    l_(l),
    h_(h),
    w_(w)
  {
    pos_[0] = cen_ + Vec3(real(l_)/2, 0, 0);
    pos_[1] = cen_ - Vec3(real(l_)/2, 0, 0);
    pos_[2] = cen_ + Vec3(0, real(h_)/2, 0);
    pos_[3] = cen_ - Vec3(0, real(h_)/2, 0);
    pos_[4] = cen_ + Vec3(0, 0, real(w_)/2);
    pos_[5] = cen_ - Vec3(0, 0, real(w_)/2);
  }

  virtual ~Cuboid() = default;

  virtual real Intersect(const Ray &ray) const override
  {
    real t = std::numeric_limits<real>::infinity();
    // std::cout<<"ori:("<<ray.ori_[0]<<","<<ray.ori_[1]<<","<<ray.ori_[2]<<")"<<std::endl;
    // std::cout<<"dir:("<<ray.dir_[0]<<","<<ray.dir_[1]<<","<<ray.dir_[2]<<")"<<std::endl;
    for (int i = 0; i < 6; ++i)
    {
      real num = (pos_[i] - ray.ori_).dot(n_[i]);
      real den = ray.dir_.dot(n_[i]);
      real tmp = num / den;
      if (den < EPS_ && tmp > 0)
      {
        if (!(tmp < t)) continue;
        Vec3 pos = ray.ori_ + ray.dir_ * tmp;
        Vec3 dis = pos - cen_;
        if ((std::abs(dis[0]) < real(l_)/2+EPS_) && (std::abs(dis[1]) < real(h_)/2+EPS_) &&
          (std::abs(dis[2]) < real(w_)/2+EPS_)) t = tmp;
      }
    }
    return t;
  }

  virtual Vec3 ClosestNormal(const Vec3 &pos) const 
  { 
    Vec3 dis = pos - cen_;
    if (std::abs(dis[0] - real(l_)/2) < EPS_) return (n_[0]).normalized();
    else if (std::abs(dis[0] + real(l_)/2) < EPS_) return (n_[1]).normalized();
    else if (std::abs(dis[1] - real(h_)/2) < EPS_) return (n_[2]).normalized();
    else if (std::abs(dis[1] + real(h_)/2) < EPS_) return (n_[3]).normalized();
    else if (std::abs(dis[2] - real(w_)/2) < EPS_) return (n_[4]).normalized();
    else if (std::abs(dis[2] + real(w_)/2) < EPS_) return (n_[5]).normalized();
  }
};

}