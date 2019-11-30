#ifndef ZXERO_ZXERO_GEOMETRY_H__
#define ZXERO_ZXERO_GEOMETRY_H__



#include "PlayerBasicInfo.pb.h"
#include "SceneConfig.pb.h"
#include "Base.pb.h"
#include "types.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <cmath>


namespace zxero {
	template <typename CoordinateType>
	using point_xy = boost::geometry::model::d2::point_xy<CoordinateType>;

	template <typename CoordinateType>
	using point_xyz = boost::geometry::model::point<CoordinateType, 3, boost::geometry::cs::cartesian>;

	template <typename CoordinateType>
	using Polygon = boost::geometry::model::polygon<point_xy<CoordinateType>>;


	template <typename PointType>
	const  point_xy<typename boost::geometry::coordinate_type<PointType>::type> point_remove_y(const PointType& origin)
	{
		boost::geometry::assert_dimension<PointType, 3>();
		return point_xy<typename boost::geometry::coordinate_type<PointType>::type>(boost::geometry::get<0>(origin),
			boost::geometry::get<2>(origin));
	}

	template <typename CoordinateType>
	struct Rect : public boost::geometry::model::polygon<point_xy<CoordinateType>>
	{
		Rect(const point_xy<CoordinateType>& bottom_left,
			const point_xy<CoordinateType>& bottom_right,
			const point_xy<CoordinateType>& up_right,
			const point_xy<CoordinateType>& up_left)
		{
			boost::geometry::append((boost::geometry::model::polygon<point_xy<CoordinateType>>&)(*this), bottom_left);
			boost::geometry::append((boost::geometry::model::polygon<point_xy<CoordinateType>>&)(*this), bottom_right);
			boost::geometry::append((boost::geometry::model::polygon<point_xy<CoordinateType>>&)(*this), up_right);
			boost::geometry::append((boost::geometry::model::polygon<point_xy<CoordinateType>>&)(*this), up_left );
			boost::geometry::append((boost::geometry::model::polygon<point_xy<CoordinateType>>&)(*this), bottom_left);//使其闭合
		}
		bool Contains(const point_xy<CoordinateType>& p) {
			return boost::geometry::within(p, (boost::geometry::model::polygon<point_xy<CoordinateType>>)(*this));
		}
	};

	template <typename CoordinateType,  typename RadianType>
	point_xy<CoordinateType> move_point_xy(const point_xy<CoordinateType>& origin, const CoordinateType& distance, const RadianType& direction_as_radian)
	{
		point_xy<CoordinateType> normalize;
		boost::geometry::set<0>(normalize, 0);
		boost::geometry::set<1>(normalize, distance);
		point_xy<CoordinateType> result;
		boost::geometry::strategy::transform::rotate_transformer<boost::geometry::radian, RadianType, 2, 2> rotate(direction_as_radian);
		ZXERO_ASSERT(boost::geometry::transform(normalize, result, rotate));
		boost::geometry::add_point(result, origin);
		return point_xy<CoordinateType>(result);
	}


	//把3d点当做2d处理, 不操作y轴
	template <typename PointType>
	PointType move_point(const PointType& origin, 
		const typename boost::geometry::coordinate_type<PointType>::type& distance, 
		const typename boost::geometry::default_distance_result<PointType>::type& radian)
	{
		typedef typename boost::geometry::coordinate_type<PointType>::type coordinate_type;
		boost::geometry::assert_dimension<PointType, 3>();
		point_xy<coordinate_type> origin2d = point_remove_y(origin);
		point_xy<coordinate_type> temp_result = move_point_xy(origin2d, distance, radian);
		PointType result;
		boost::geometry::set<0>(result, boost::geometry::get<0>(temp_result));
		boost::geometry::set<1>(result, boost::geometry::get<1>(origin));
		boost::geometry::set<2>(result, boost::geometry::get<1>(temp_result));
		return result;
	}

	template <typename PointType>
	PointType move_point(const PointType& current,
		const PointType& target,
		const typename boost::geometry::coordinate_type<PointType>::type& distance
		)
	{
		typename boost::geometry::default_distance_result<PointType>::type radian =
			get_direction(current, target);
		return move_point(current, distance, radian);
	}

	//把3d点当做2d处理, 不操作y轴
	template <typename PointType>
	typename boost::geometry::default_distance_result<PointType>::type get_direction(const PointType& first, const PointType& second) {
		typedef typename boost::geometry::coordinate_type<PointType>::type coordinate_type;
		typedef typename boost::geometry::default_distance_result<PointType, PointType>::type result_type;
		point_xy<coordinate_type> normalize(coordinate_type(0), coordinate_type(1)); //法线
		point_xy<coordinate_type> origin(coordinate_type(0), coordinate_type(0)); //原点
		point_xy<coordinate_type> first2d = point_remove_y(first);
		point_xy<coordinate_type> second2d = point_remove_y(second);
		point_xy<coordinate_type> dir_vector = second2d;
		boost::geometry::subtract_point(dir_vector, first2d);
		auto dot_product = boost::geometry::dot_product(normalize, dir_vector);
		auto dir_vector_len = boost::geometry::distance(origin, dir_vector);
		auto normalize_len = 1; //法线长度为1, 不用算了
		auto cos_value = dot_product / (dir_vector_len * normalize_len);
		auto radian = std::acos(cos_value);

		if (boost::geometry::get<0>(dir_vector) > 0) { //一三象限
			return (result_type)radian;
		}
		else {//二四象限
			return -(result_type)radian;
		}
	}

	template <typename PointType>
	typename boost::geometry::coordinate_type<PointType>::type distance2d(const PointType& p1, const PointType& p2) {
		boost::geometry::assert_dimension<PointType, 3>();
		typedef typename boost::geometry::coordinate_type<PointType>::type coordinate_type;
		return  coordinate_type(boost::geometry::distance(point_remove_y(p1), point_remove_y(p2)));
	}

	const Packet::Position string2pos(std::string pos);
}


namespace boost {
	namespace geometry {
		namespace traits
		{
			using namespace zxero;
			template<>
			struct tag<Packet::Position>
			{
				typedef point_tag type;
			};
			template<>
			struct coordinate_type<Packet::Position>
			{
				typedef  int32 type;
			};
			template<>
			struct coordinate_system<Packet::Position>
			{
				typedef cs::cartesian type;
			};
			template<>
			struct dimension<Packet::Position>
				: boost::mpl::int_<3>
			{};
			template <std::size_t Dimension>
			struct access<Packet::Position, Dimension>
			{

				static inline int32 get(Packet::Position const& /*p*/) { return 0; }
				static inline int32 set(Packet::Position& /*p*/, const float& /*value*/) { return 0; }
			};
			template<>
			struct access<Packet::Position, 0>
			{
				static inline int32 get(
					Packet::Position const& p)
				{
					return p.x();
				}

				static inline void set(Packet::Position& p,
					int32 const& value)
				{
					p.set_x(value);
				}
			};
			template<>
			struct access<Packet::Position, 1>
			{
				static inline int32 get(
					Packet::Position const& p)
				{
					return p.y();
				}

				static inline void set(Packet::Position& p,
					int32 const& value)
				{
					p.set_y(value);
				}
			};
			template<>
			struct access<Packet::Position, 2>
			{
				static inline int32 get(
					Packet::Position const& p)
				{
					return p.z();
				}

				static inline void set(Packet::Position& p,
					int32 const& value)
				{
					p.set_z(value);
				}
			};
		}
	}
}
#endif // ZXERO_ZXERO_GEOMETRY_H__