pt1={}
pt1.x = 5
pt1.y = 20
pt2={}
pt2.x = 5
pt2.y = 100
pt3={}
pt3.x = 123
pt3.y = 100
pt4={}
pt4.x = 123
pt4.y = 20
ctrl_pts={}
ctrl_pts[1] = pt1
ctrl_pts[2] = pt2
ctrl_pts[3] = pt3
ctrl_pts[4] = pt4

function _update()
end

function _draw()
 cls()
 for pt in all(ctrl_pts) do
  circfill(pt.x, pt.y, 2, 14)
  end

  for x=1,100 do
   t = x / 100
   pt = bezier(ctrl_pts, t)
   pset(pt.x, pt.y, 7)
  end
end

function _init()
end

function bezier(points, t)
 pa = lerp(points[1], points[2], t)
 pb = lerp(points[2], points[3], t)
 pc = lerp(points[3], points[4], t)
 pabbc = lerp(pa, pb, t)
 pbccd = lerp(pb, pc, t)
 return lerp(pabbc, pbccd, t)
end

function lerp(pta, ptb, t)
 pt = {}
 pt.x = pta.x + (ptb.x - pta.x) * t
 pt.y = pta.y + (ptb.y - pta.y) * t
 return pt
end
