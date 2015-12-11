-- animated bezier
-- by groovestomp
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

x = 0
currpt={}
incr=1

function _update()
end

function _draw()
 cls()

	for x=1,200 do
	 t = x / 200
	 pt = bezier(ctrl_pts, t, false)
	 pset(pt.x, pt.y, 7)
	end

 x+=incr
 if (x>=100) incr=-1
 if (x<=0) incr=1
 currpt = bezier(ctrl_pts, x/100, true)

 for pt in all(ctrl_pts) do
		circfill(pt.x, pt.y, 2, 14)
	end
	circfill(currpt.x, currpt.y, 2, 6)
end

function _init()
end

function bezier(points, t, b)
 pa = lerp(points[1], points[2], t)
 if (b) line(points[1].x, points[1].y, points[2].x, points[2].y, 11)
 pb = lerp(points[2], points[3], t)
 if (b) line(points[2].x, points[2].y, points[3].x, points[3].y, 11)
 pc = lerp(points[3], points[4], t)
 if (b) line(points[3].x, points[3].y, points[4].x, points[4].y, 11)
 pabbc = lerp(pa, pb, t)
 if (b) line(pa.x, pa.y, pb.x, pb.y, 12)
 pbccd = lerp(pb, pc, t)
 if (b) line(pb.x, pb.y, pc.x, pc.y, 12)
 last = lerp(pabbc, pbccd, t)
 if (b) line(pabbc.x, pabbc.y, pbccd.x, pbccd.y, 13)
 return last
end

function lerp(pta, ptb, t)
 pt = {}
 pt.x = pta.x + (ptb.x - pta.x) * t
 pt.y = pta.y + (ptb.y - pta.y) * t
 return pt
end