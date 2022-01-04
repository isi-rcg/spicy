// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
//
// Copyright (C) 2018, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.

#include "precomp.hpp"
#include "opencv2/objdetect.hpp"

#include <limits>
#include <cmath>
#include <iostream>

namespace cv
{
using std::vector;

class QRDecode
{
public:
    void init(Mat src, double eps_vertical_ = 0.2, double eps_horizontal_ = 0.1);
    bool localization();
    bool transformation();
    Mat getBinBarcode() { return bin_barcode; }
    Mat getStraightBarcode() { return straight_barcode; }
    vector<Point2f> getTransformationPoints() { return transformation_points; }
protected:
    bool computeTransformationPoints();
    vector<Vec3d> searchVerticalLines();
    vector<Point2f> separateHorizontalLines(vector<Vec3d> list_lines);
    void fixationPoints(vector<Point2f> &local_point);
    Point2f intersectionLines(Point2f a1, Point2f a2, Point2f b1, Point2f b2);
    vector<Point2f> getQuadrilateral(vector<Point2f> angle_list);
    bool testBypassRoute(vector<Point2f> hull, int start, int finish);
    inline double getCosVectors(Point2f a, Point2f b, Point2f c);

    Mat barcode, bin_barcode, straight_barcode;
    vector<Point2f> localization_points, transformation_points;
    double eps_vertical, eps_horizontal, coeff_expansion;
};


void QRDecode::init(Mat src, double eps_vertical_, double eps_horizontal_)
{
    double min_side = std::min(src.size().width, src.size().height);
    if (min_side < 512.0)
    {
        coeff_expansion = 512.0 / min_side;
        int width  = static_cast<int>(src.size().width  * coeff_expansion);
        int height = static_cast<int>(src.size().height  * coeff_expansion);
        Size new_size(width, height);
        resize(src, barcode, new_size);
    }
    else
    {
        coeff_expansion = 1.0;
        barcode = src;
    }
    eps_vertical   = eps_vertical_;
    eps_horizontal = eps_horizontal_;
    adaptiveThreshold(barcode, bin_barcode, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 71, 2);
}

vector<Vec3d> QRDecode::searchVerticalLines()
{
    vector<Vec3d> result;
    int temp_length = 0;
    uint8_t next_pixel, future_pixel;
    vector<double> test_lines;

    for (int x = 0; x < bin_barcode.rows; x++)
    {
        for (int y = 0; y < bin_barcode.cols; y++)
        {
            if (bin_barcode.at<uint8_t>(x, y) > 0) { continue; }

            // --------------- Search vertical lines --------------- //

            test_lines.clear();
            future_pixel = 255;

            for (int i = x; i < bin_barcode.rows - 1; i++)
            {
                next_pixel = bin_barcode.at<uint8_t>(i + 1, y);
                temp_length++;
                if (next_pixel == future_pixel)
                {
                    future_pixel = 255 - future_pixel;
                    test_lines.push_back(temp_length);
                    temp_length = 0;
                    if (test_lines.size() == 5) { break; }
                }
            }

            // --------------- Compute vertical lines --------------- //

            if (test_lines.size() == 5)
            {
                double length = 0.0, weight = 0.0;

                for (size_t i = 0; i < test_lines.size(); i++) { length += test_lines[i]; }

                CV_Assert(length > 0);
                for (size_t i = 0; i < test_lines.size(); i++)
                {
                    if (i == 2) { weight += fabs((test_lines[i] / length) - 3.0/7.0); }
                    else        { weight += fabs((test_lines[i] / length) - 1.0/7.0); }
                }

                if (weight < eps_vertical)
                {
                    Vec3d line;
                    line[0] = x; line[1] = y, line[2] = length;
                    result.push_back(line);
                }
            }
        }
    }
    return result;
}

vector<Point2f> QRDecode::separateHorizontalLines(vector<Vec3d> list_lines)
{
    vector<Vec3d> result;
    int temp_length = 0;
    uint8_t next_pixel, future_pixel;
    vector<double> test_lines;

    for (size_t pnt = 0; pnt < list_lines.size(); pnt++)
    {
        int x = static_cast<int>(list_lines[pnt][0] + list_lines[pnt][2] * 0.5);
        int y = static_cast<int>(list_lines[pnt][1]);

        // --------------- Search horizontal up-lines --------------- //
        test_lines.clear();
        future_pixel = 255;

        for (int j = y; j < bin_barcode.cols - 1; j++)
        {
            next_pixel = bin_barcode.at<uint8_t>(x, j + 1);
            temp_length++;
            if (next_pixel == future_pixel)
            {
                future_pixel = 255 - future_pixel;
                test_lines.push_back(temp_length);
                temp_length = 0;
                if (test_lines.size() == 3) { break; }
            }
        }

        // --------------- Search horizontal down-lines --------------- //
        future_pixel = 255;

        for (int j = y; j >= 1; j--)
        {
            next_pixel = bin_barcode.at<uint8_t>(x, j - 1);
            temp_length++;
            if (next_pixel == future_pixel)
            {
                future_pixel = 255 - future_pixel;
                test_lines.push_back(temp_length);
                temp_length = 0;
                if (test_lines.size() == 6) { break; }
            }
        }

        // --------------- Compute horizontal lines --------------- //

        if (test_lines.size() == 6)
        {
            double length = 0.0, weight = 0.0;

            for (size_t i = 0; i < test_lines.size(); i++) { length += test_lines[i]; }

            CV_Assert(length > 0);
            for (size_t i = 0; i < test_lines.size(); i++)
            {
                if (i % 3 == 0) { weight += fabs((test_lines[i] / length) - 3.0/14.0); }
                else            { weight += fabs((test_lines[i] / length) - 1.0/ 7.0); }
            }

            if(weight < eps_horizontal)
            {
                result.push_back(list_lines[pnt]);
            }

        }
    }

    vector<Point2f> point2f_result;
    for (size_t i = 0; i < result.size(); i++)
    {
        point2f_result.push_back(
              Point2f(static_cast<float>(result[i][1]),
                      static_cast<float>(result[i][0] + result[i][2] * 0.5)));
    }
    return point2f_result;
}

void QRDecode::fixationPoints(vector<Point2f> &local_point)
{
    double cos_angles[3], norm_triangl[3];

    norm_triangl[0] = norm(local_point[1] - local_point[2]);
    norm_triangl[1] = norm(local_point[0] - local_point[2]);
    norm_triangl[2] = norm(local_point[1] - local_point[0]);

    cos_angles[0] = (pow(norm_triangl[1], 2) + pow(norm_triangl[2], 2) - pow(norm_triangl[0], 2))
    / (2 * norm_triangl[1] * norm_triangl[2]);
    cos_angles[1] = (pow(norm_triangl[0], 2) + pow(norm_triangl[2], 2) - pow(norm_triangl[1], 2))
    / (2 * norm_triangl[0] * norm_triangl[2]);
    cos_angles[2] = (pow(norm_triangl[0], 2) + pow(norm_triangl[1], 2) - pow(norm_triangl[2], 2))
    / (2 * norm_triangl[0] * norm_triangl[1]);

    size_t i_min_cos =
    (cos_angles[0] < cos_angles[1] && cos_angles[0] < cos_angles[2]) ? 0 :
    (cos_angles[1] < cos_angles[0] && cos_angles[1] < cos_angles[2]) ? 1 : 2;

    std::swap(local_point[0], local_point[i_min_cos]);

    Point2f rpt = local_point[0], bpt = local_point[1], gpt = local_point[2];
    Matx22f m(rpt.x - bpt.x, rpt.y - bpt.y, gpt.x - rpt.x, gpt.y - rpt.y);
    if( determinant(m) > 0 )
    {
        std::swap(local_point[1], local_point[2]);
    }
}

bool QRDecode::localization()
{
    Point2f begin, end;
    vector<Vec3d> list_lines_x = searchVerticalLines();
    if( list_lines_x.empty() ) { return false; }
    vector<Point2f> list_lines_y = separateHorizontalLines(list_lines_x);
    if( list_lines_y.empty() ) { return false; }

    vector<Point2f> centers;
    Mat labels;
    if (list_lines_y.size() < 3) { return false; }
    kmeans(list_lines_y, 3, labels,
           TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0),
           3, KMEANS_PP_CENTERS, localization_points);

    fixationPoints(localization_points);
    if (localization_points.size() != 3) { return false; }

    if (coeff_expansion > 1.0)
    {
        int width  = static_cast<int>(bin_barcode.size().width  / coeff_expansion);
        int height = static_cast<int>(bin_barcode.size().height / coeff_expansion);
        Size new_size(width, height);
        Mat intermediate;
        resize(bin_barcode, intermediate, new_size);
        bin_barcode = intermediate.clone();
        for (size_t i = 0; i < localization_points.size(); i++)
        {
            localization_points[i] /= coeff_expansion;
        }
    }

    for (size_t i = 0; i < localization_points.size(); i++)
    {
        for (size_t j = i + 1; j < localization_points.size(); j++)
        {
            if (norm(localization_points[i] - localization_points[j]) < 10)
            {
                return false;
            }
        }
    }
    return true;

}

bool QRDecode::computeTransformationPoints()
{
    if (localization_points.size() != 3) { return false; }

    vector<Point> locations, non_zero_elem[3], newHull;
    vector<Point2f> new_non_zero_elem[3];
    for (size_t i = 0; i < 3; i++)
    {
        Mat mask = Mat::zeros(bin_barcode.rows + 2, bin_barcode.cols + 2, CV_8UC1);
        uint8_t next_pixel, future_pixel = 255;
        int count_test_lines = 0, index = static_cast<int>(localization_points[i].x);
        for (; index < bin_barcode.cols - 1; index++)
        {
            next_pixel = bin_barcode.at<uint8_t>(
                            static_cast<int>(localization_points[i].y), index + 1);
            if (next_pixel == future_pixel)
            {
                future_pixel = 255 - future_pixel;
                count_test_lines++;
                if (count_test_lines == 2)
                {
                    floodFill(bin_barcode, mask,
                              Point(index + 1, static_cast<int>(localization_points[i].y)), 255,
                              0, Scalar(), Scalar(), FLOODFILL_MASK_ONLY);
                    break;
                }
            }
        }
        Mat mask_roi = mask(Range(1, bin_barcode.rows - 1), Range(1, bin_barcode.cols - 1));
        findNonZero(mask_roi, non_zero_elem[i]);
        newHull.insert(newHull.end(), non_zero_elem[i].begin(), non_zero_elem[i].end());
    }
    convexHull(Mat(newHull), locations);
    for (size_t i = 0; i < locations.size(); i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            for (size_t k = 0; k < non_zero_elem[j].size(); k++)
            {
                if (locations[i] == non_zero_elem[j][k])
                {
                    new_non_zero_elem[j].push_back(locations[i]);
                }
            }
        }
    }

    double pentagon_diag_norm = -1;
    Point2f down_left_edge_point, up_right_edge_point, up_left_edge_point;
    for (size_t i = 0; i < new_non_zero_elem[1].size(); i++)
    {
        for (size_t j = 0; j < new_non_zero_elem[2].size(); j++)
        {
            double temp_norm = norm(new_non_zero_elem[1][i] - new_non_zero_elem[2][j]);
            if (temp_norm > pentagon_diag_norm)
            {
                down_left_edge_point = new_non_zero_elem[1][i];
                up_right_edge_point  = new_non_zero_elem[2][j];
                pentagon_diag_norm = temp_norm;
            }
        }
    }

    if (down_left_edge_point == Point2f(0, 0) ||
        up_right_edge_point  == Point2f(0, 0) ||
        new_non_zero_elem[0].size() == 0) { return false; }

    double max_area = -1;
    up_left_edge_point = new_non_zero_elem[0][0];

    for (size_t i = 0; i < new_non_zero_elem[0].size(); i++)
    {
        vector<Point2f> list_edge_points;
        list_edge_points.push_back(new_non_zero_elem[0][i]);
        list_edge_points.push_back(down_left_edge_point);
        list_edge_points.push_back(up_right_edge_point);

        double temp_area = fabs(contourArea(list_edge_points));

        if (max_area < temp_area)
        {
            up_left_edge_point = new_non_zero_elem[0][i];
            max_area = temp_area;
        }
    }

    Point2f down_max_delta_point, up_max_delta_point;
    double norm_down_max_delta = -1, norm_up_max_delta = -1;
    for (size_t i = 0; i < new_non_zero_elem[1].size(); i++)
    {
        double temp_norm_delta = norm(up_left_edge_point - new_non_zero_elem[1][i])
                               + norm(down_left_edge_point - new_non_zero_elem[1][i]);
        if (norm_down_max_delta < temp_norm_delta)
        {
            down_max_delta_point = new_non_zero_elem[1][i];
            norm_down_max_delta = temp_norm_delta;
        }
    }


    for (size_t i = 0; i < new_non_zero_elem[2].size(); i++)
    {
        double temp_norm_delta = norm(up_left_edge_point - new_non_zero_elem[2][i])
                               + norm(up_right_edge_point - new_non_zero_elem[2][i]);
        if (norm_up_max_delta < temp_norm_delta)
        {
            up_max_delta_point = new_non_zero_elem[2][i];
            norm_up_max_delta = temp_norm_delta;
        }
    }

    transformation_points.push_back(down_left_edge_point);
    transformation_points.push_back(up_left_edge_point);
    transformation_points.push_back(up_right_edge_point);
    transformation_points.push_back(
        intersectionLines(down_left_edge_point, down_max_delta_point,
                          up_right_edge_point, up_max_delta_point));

    vector<Point2f> quadrilateral = getQuadrilateral(transformation_points);
    transformation_points = quadrilateral;

    return true;
}

Point2f QRDecode::intersectionLines(Point2f a1, Point2f a2, Point2f b1, Point2f b2)
{
    Point2f result_square_angle(
                              ((a1.x * a2.y  -  a1.y * a2.x) * (b1.x - b2.x) -
                               (b1.x * b2.y  -  b1.y * b2.x) * (a1.x - a2.x)) /
                              ((a1.x - a2.x) * (b1.y - b2.y) -
                               (a1.y - a2.y) * (b1.x - b2.x)),
                              ((a1.x * a2.y  -  a1.y * a2.x) * (b1.y - b2.y) -
                               (b1.x * b2.y  -  b1.y * b2.x) * (a1.y - a2.y)) /
                              ((a1.x - a2.x) * (b1.y - b2.y) -
                               (a1.y - a2.y) * (b1.x - b2.x))
                              );
    return result_square_angle;
}

// test function (if true then ------> else <------ )
bool QRDecode::testBypassRoute(vector<Point2f> hull, int start, int finish)
{
    int index_hull = start, next_index_hull, hull_size = (int)hull.size();
    double test_length[2] = { 0.0, 0.0 };
    do
    {
        next_index_hull = index_hull + 1;
        if (next_index_hull == hull_size) { next_index_hull = 0; }
        test_length[0] += norm(hull[index_hull] - hull[next_index_hull]);
        index_hull = next_index_hull;
    }
    while(index_hull != finish);

    index_hull = start;
    do
    {
        next_index_hull = index_hull - 1;
        if (next_index_hull == -1) { next_index_hull = hull_size - 1; }
        test_length[1] += norm(hull[index_hull] - hull[next_index_hull]);
        index_hull = next_index_hull;
    }
    while(index_hull != finish);

    if (test_length[0] < test_length[1]) { return true; } else { return false; }
}

vector<Point2f> QRDecode::getQuadrilateral(vector<Point2f> angle_list)
{
    size_t angle_size = angle_list.size();
    uint8_t value, mask_value;
    Mat mask = Mat::zeros(bin_barcode.rows + 2, bin_barcode.cols + 2, CV_8UC1);
    Mat fill_bin_barcode = bin_barcode.clone();
    for (size_t i = 0; i < angle_size; i++)
    {
        LineIterator line_iter(bin_barcode, angle_list[ i      % angle_size],
                                            angle_list[(i + 1) % angle_size]);
        for(int j = 0; j < line_iter.count; j++, ++line_iter)
        {
            value = bin_barcode.at<uint8_t>(line_iter.pos());
            mask_value = mask.at<uint8_t>(line_iter.pos() + Point(1, 1));
            if (value == 0 && mask_value == 0)
            {
                floodFill(fill_bin_barcode, mask, line_iter.pos(), 255,
                          0, Scalar(), Scalar(), FLOODFILL_MASK_ONLY);
            }
        }
    }
    vector<Point> locations;
    Mat mask_roi = mask(Range(1, bin_barcode.rows - 1), Range(1, bin_barcode.cols - 1));

    cv::findNonZero(mask_roi, locations);

    for (size_t i = 0; i < angle_list.size(); i++)
    {
        int x = static_cast<int>(angle_list[i].x);
        int y = static_cast<int>(angle_list[i].y);
        locations.push_back(Point(x, y));
    }

    vector<Point> integer_hull;
    convexHull(Mat(locations), integer_hull);
    int hull_size = (int)integer_hull.size();
    vector<Point2f> hull(hull_size);
    for (int i = 0; i < hull_size; i++)
    {
        float x = static_cast<float>(integer_hull[i].x);
        float y = static_cast<float>(integer_hull[i].y);
        hull[i] = Point2f(x, y);
    }

    const double experimental_area = fabs(contourArea(hull));

    vector<Point2f> result_hull_point(angle_size);
    double min_norm;
    for (size_t i = 0; i < angle_size; i++)
    {
        min_norm = std::numeric_limits<double>::max();
        Point closest_pnt;
        for (int j = 0; j < hull_size; j++)
        {
            double temp_norm = norm(hull[j] - angle_list[i]);
            if (min_norm > temp_norm)
            {
                min_norm = temp_norm;
                closest_pnt = hull[j];
            }
        }
        result_hull_point[i] = closest_pnt;
    }

    int start_line[2] = { 0, 0 }, finish_line[2] = { 0, 0 }, unstable_pnt = 0;
    for (int i = 0; i < hull_size; i++)
    {
        if (result_hull_point[2] == hull[i]) { start_line[0] = i; }
        if (result_hull_point[1] == hull[i]) { finish_line[0] = start_line[1] = i; }
        if (result_hull_point[0] == hull[i]) { finish_line[1] = i; }
        if (result_hull_point[3] == hull[i]) { unstable_pnt = i; }
    }

    int index_hull, extra_index_hull, next_index_hull, extra_next_index_hull;
    Point result_side_begin[4], result_side_end[4];

    bool bypass_orientation = testBypassRoute(hull, start_line[0], finish_line[0]);
    bool extra_bypass_orientation;

    min_norm = std::numeric_limits<double>::max();
    index_hull = start_line[0];
    do
    {
        if (bypass_orientation) { next_index_hull = index_hull + 1; }
        else { next_index_hull = index_hull - 1; }

        if (next_index_hull == hull_size) { next_index_hull = 0; }
        if (next_index_hull == -1) { next_index_hull = hull_size - 1; }

        Point angle_closest_pnt =  norm(hull[index_hull] - angle_list[1]) >
        norm(hull[index_hull] - angle_list[2]) ? angle_list[2] : angle_list[1];

        Point intrsc_line_hull =
        intersectionLines(hull[index_hull], hull[next_index_hull],
                          angle_list[1], angle_list[2]);
        double temp_norm = getCosVectors(hull[index_hull], intrsc_line_hull, angle_closest_pnt);
        if (min_norm > temp_norm &&
            norm(hull[index_hull] - hull[next_index_hull]) >
            norm(angle_list[1] - angle_list[2]) * 0.1)
        {
            min_norm = temp_norm;
            result_side_begin[0] = hull[index_hull];
            result_side_end[0]   = hull[next_index_hull];
        }


        index_hull = next_index_hull;
    }
    while(index_hull != finish_line[0]);

    if (min_norm == std::numeric_limits<double>::max())
    {
        result_side_begin[0] = angle_list[1];
        result_side_end[0]   = angle_list[2];
    }

    min_norm = std::numeric_limits<double>::max();
    index_hull = start_line[1];
    bypass_orientation = testBypassRoute(hull, start_line[1], finish_line[1]);
    do
    {
        if (bypass_orientation) { next_index_hull = index_hull + 1; }
        else { next_index_hull = index_hull - 1; }

        if (next_index_hull == hull_size) { next_index_hull = 0; }
        if (next_index_hull == -1) { next_index_hull = hull_size - 1; }

        Point angle_closest_pnt =  norm(hull[index_hull] - angle_list[0]) >
        norm(hull[index_hull] - angle_list[1]) ? angle_list[1] : angle_list[0];

        Point intrsc_line_hull =
        intersectionLines(hull[index_hull], hull[next_index_hull],
                          angle_list[0], angle_list[1]);
        double temp_norm = getCosVectors(hull[index_hull], intrsc_line_hull, angle_closest_pnt);
        if (min_norm > temp_norm &&
            norm(hull[index_hull] - hull[next_index_hull]) >
            norm(angle_list[0] - angle_list[1]) * 0.05)
        {
            min_norm = temp_norm;
            result_side_begin[1] = hull[index_hull];
            result_side_end[1]   = hull[next_index_hull];
        }

        index_hull = next_index_hull;
    }
    while(index_hull != finish_line[1]);

    if (min_norm == std::numeric_limits<double>::max())
    {
        result_side_begin[1] = angle_list[0];
        result_side_end[1]   = angle_list[1];
    }

    bypass_orientation = testBypassRoute(hull, start_line[0], unstable_pnt);
    extra_bypass_orientation = testBypassRoute(hull, finish_line[1], unstable_pnt);

    vector<Point2f> result_angle_list(4), test_result_angle_list(4);
    double min_diff_area = std::numeric_limits<double>::max(), test_diff_area;
    index_hull = start_line[0];
    double standart_norm = std::max(
        norm(result_side_begin[0] - result_side_end[0]),
        norm(result_side_begin[1] - result_side_end[1]));
    do
    {
        if (bypass_orientation) { next_index_hull = index_hull + 1; }
        else { next_index_hull = index_hull - 1; }

        if (next_index_hull == hull_size) { next_index_hull = 0; }
        if (next_index_hull == -1) { next_index_hull = hull_size - 1; }

        if (norm(hull[index_hull] - hull[next_index_hull]) < standart_norm * 0.1)
        { index_hull = next_index_hull; continue; }

        extra_index_hull = finish_line[1];
        do
        {
            if (extra_bypass_orientation) { extra_next_index_hull = extra_index_hull + 1; }
            else { extra_next_index_hull = extra_index_hull - 1; }

            if (extra_next_index_hull == hull_size) { extra_next_index_hull = 0; }
            if (extra_next_index_hull == -1) { extra_next_index_hull = hull_size - 1; }

            if (norm(hull[extra_index_hull] - hull[extra_next_index_hull]) < standart_norm * 0.1)
            { extra_index_hull = extra_next_index_hull; continue; }

            test_result_angle_list[0]
            = intersectionLines(result_side_begin[0], result_side_end[0],
                                result_side_begin[1], result_side_end[1]);
            test_result_angle_list[1]
            = intersectionLines(result_side_begin[1], result_side_end[1],
                                hull[extra_index_hull], hull[extra_next_index_hull]);
            test_result_angle_list[2]
            = intersectionLines(hull[extra_index_hull], hull[extra_next_index_hull],
                                hull[index_hull], hull[next_index_hull]);
            test_result_angle_list[3]
            = intersectionLines(hull[index_hull], hull[next_index_hull],
                                result_side_begin[0], result_side_end[0]);

            test_diff_area = fabs(fabs(contourArea(test_result_angle_list)) - experimental_area);
            if (min_diff_area > test_diff_area)
            {
                min_diff_area = test_diff_area;
                for (size_t i = 0; i < test_result_angle_list.size(); i++)
                {
                    result_angle_list[i] = test_result_angle_list[i];
                }
            }

            extra_index_hull = extra_next_index_hull;
        }
        while(extra_index_hull != unstable_pnt);

        index_hull = next_index_hull;
    }
    while(index_hull != unstable_pnt);

    if (norm(result_angle_list[0] - angle_list[1]) > 2) { result_angle_list[0] = angle_list[1]; }
    if (norm(result_angle_list[1] - angle_list[0]) > 2) { result_angle_list[1] = angle_list[0]; }
    if (norm(result_angle_list[3] - angle_list[2]) > 2) { result_angle_list[3] = angle_list[2]; }

    return result_angle_list;
}

//      / | b
//     /  |
//    /   |
//  a/    | c

inline double QRDecode::getCosVectors(Point2f a, Point2f b, Point2f c)
{
    return ((a - b).x * (c - b).x + (a - b).y * (c - b).y) / (norm(a - b) * norm(c - b));
}

bool QRDecode::transformation()
{
    if(!computeTransformationPoints()) { return false; }

    double max_length_norm = -1;
    size_t transform_size = transformation_points.size();
    for (size_t i = 0; i < transform_size; i++)
    {
        double len_norm = norm(transformation_points[i % transform_size] -
                               transformation_points[(i + 1) % transform_size]);
        max_length_norm = std::max(max_length_norm, len_norm);
    }

    Point2f transformation_points_[] =
    {
        transformation_points[0],
        transformation_points[1],
        transformation_points[2],
        transformation_points[3]
    };

    Point2f perspective_points[] =
    {
        Point2f(0.f, 0.f), Point2f(0.f, (float)max_length_norm),
        Point2f((float)max_length_norm, (float)max_length_norm),
        Point2f((float)max_length_norm, 0.f)
    };

    Mat H = getPerspectiveTransform(transformation_points_, perspective_points);

    warpPerspective(bin_barcode, straight_barcode, H,
                    Size(static_cast<int>(max_length_norm),
                         static_cast<int>(max_length_norm)));
    return true;
}


struct QRCodeDetector::Impl
{
public:
    Impl() { epsX = 0.2; epsY = 0.1; }
    ~Impl() {}

    double epsX, epsY;
};

QRCodeDetector::QRCodeDetector() : p(new Impl) {}
QRCodeDetector::~QRCodeDetector() {}

void QRCodeDetector::setEpsX(double epsX) { p->epsX = epsX; }
void QRCodeDetector::setEpsY(double epsY) { p->epsY = epsY; }

bool QRCodeDetector::detect(InputArray in, OutputArray points) const
{
    Mat inarr = in.getMat();
    CV_Assert(!inarr.empty());
    CV_Assert(inarr.type() == CV_8UC1);
    QRDecode qrdec;
    qrdec.init(inarr, p->epsX, p->epsY);
    if (!qrdec.localization()) { return false; }
    if (!qrdec.transformation()) { return false; }
    vector<Point2f> pnts2f = qrdec.getTransformationPoints();
    Mat(pnts2f).convertTo(points, points.fixedType() ? points.type() : CV_32FC2);
    return true;
}

CV_EXPORTS bool detectQRCode(InputArray in, std::vector<Point> &points, double eps_x, double eps_y)
{
    QRCodeDetector qrdetector;
    qrdetector.setEpsX(eps_x);
    qrdetector.setEpsY(eps_y);

    return qrdetector.detect(in, points);
}

}
