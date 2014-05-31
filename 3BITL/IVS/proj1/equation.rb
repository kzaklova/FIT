#use version 1.9.3 (ruby1.9.3)
class Equation
  def self.solve_quadratic(a, b, c)
    if a == 0
      # a, b => 0, no solution
      if b == 0
        return nil
      # linear equation
      else
        return [-c/b.to_f]
      end
    end
    # discriminant
    dis = (b*b)-(4*a*c)
    # one solution
    if dis == 0
      return [-b/2*a.to_f]
    # two solutions
    elsif dis > 0
      return [(-b+Math.sqrt(dis))/(2*a).to_f, (-b-Math.sqrt(dis))/(2*a).to_f]
    # no solution
    else
      return nil
    end
  end
end
