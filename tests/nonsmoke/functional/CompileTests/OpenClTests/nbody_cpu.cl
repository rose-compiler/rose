//
// File:       nbody_cpu.cl
//
// Abstract:   This example performs an NBody simulation which calculates a gravity field 
//             and corresponding velocity and acceleration contributions accumulated 
//             by each body in the system from every other body.  This example
//             also shows how to mitigate computation between all available devices
//             including CPU and GPU devices, as well as a hybrid combination of both,
//             using separate threads for each simulator.
//
// Version:    <1.0>
//
// Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc. ("Apple")
//             in consideration of your agreement to the following terms, and your use,
//             installation, modification or redistribution of this Apple software
//             constitutes acceptance of these terms.  If you do not agree with these
//             terms, please do not use, install, modify or redistribute this Apple
//             software.
//
//             In consideration of your agreement to abide by the following terms, and
//             subject to these terms, Apple grants you a personal, non - exclusive
//             license, under Apple's copyrights in this original Apple software ( the
//             "Apple Software" ), to use, reproduce, modify and redistribute the Apple
//             Software, with or without modifications, in source and / or binary forms;
//             provided that if you redistribute the Apple Software in its entirety and
//             without modifications, you must retain this notice and the following text
//             and disclaimers in all such redistributions of the Apple Software. Neither
//             the name, trademarks, service marks or logos of Apple Inc. may be used to
//             endorse or promote products derived from the Apple Software without specific
//             prior written permission from Apple.  Except as expressly stated in this
//             notice, no other rights or licenses, express or implied, are granted by
//             Apple herein, including but not limited to any patent rights that may be
//             infringed by your derivative works or by other works in which the Apple
//             Software may be incorporated.
//
//             The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
//             WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
//             WARRANTIES OF NON - INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
//             PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION
//             ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
//
//             IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
//             CONSEQUENTIAL DAMAGES ( INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//             SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//             INTERRUPTION ) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION
//             AND / OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER
//             UNDER THEORY OF CONTRACT, TORT ( INCLUDING NEGLIGENCE ), STRICT LIABILITY OR
//             OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright ( C ) 2008 Apple Inc. All Rights Reserved.
//

////////////////////////////////////////////////////////////////////////////////

__kernel void
IntegrateSystemNonVectorized(
    __global float *output_position_x, 
    __global float *output_position_y,
    __global float *output_position_z,
    __global float *mass,
    __global float *output_velocity_x, 
    __global float *output_velocity_y, 
    __global float *output_velocity_z,
    __global float *input_position_x, 
    __global float *input_position_y, 
    __global float *input_position_z,
    __global float *input_velocity_x, 
    __global float *input_velocity_y, 
    __global float *input_velocity_z,
    float time_delta, 
    float damping, 
    float softening, 
    int body_count, 
    int body_count_per_group,
    __global float4 *output_position, 
    int start_index )
{
    int index = get_global_id(0);

    float softening_squared = softening * softening;
    float position_x, position_y, position_z, m;
    float current_x1, current_y1, current_z1, current_mass1;
    float current_x2, current_y2, current_z2, current_mass2;
    float velocity_x, velocity_y, velocity_z;
    float zero = 0.0f;

    int i, j, k, l;

    int inner_loop_count = body_count;
    int outer_loop_count = body_count_per_group;
    int start = index * outer_loop_count + start_index;
    int offset = index * body_count_per_group + start_index;

    for (l = 0; l < outer_loop_count; l++)
    {

        k = l + start;

        position_x = input_position_x[k];
        position_y = input_position_y[k];
        position_z = input_position_z[k];
        m = mass[k];
        
        float final_ax = zero;
        float final_ay = zero;
        float final_az = zero;

        current_x1 = position_x;
        current_y1 = position_y;
        current_z1 = position_z;
        current_mass1 = m;

        float acceleration_x1 = zero;
        float acceleration_y1 = zero;
        float acceleration_z1 = zero;

        for (i = 0; i < inner_loop_count; i++)
        {

            float dx = input_position_x[i] - current_x1;
            float dy = input_position_y[i] - current_y1;
            float dz = input_position_z[i] - current_z1;

            float mi = mass[i];

            float distance_squared = dx * dx + dy * dy + dz * dz;
            distance_squared += softening_squared;

            float inverse_distance = native_rsqrt(distance_squared); 
            float s = (mi * inverse_distance) * (inverse_distance * inverse_distance);

            acceleration_x1 += dx * s;
            acceleration_y1 += dy * s;
            acceleration_z1 += dz * s;

        }

        final_ax = acceleration_x1;
        final_ay = acceleration_y1;
        final_az = acceleration_z1;

        velocity_x = input_velocity_x[k];
        velocity_y = input_velocity_y[k];
        velocity_z = input_velocity_z[k];

        velocity_x += final_ax * time_delta;
        velocity_y += final_ay * time_delta;
        velocity_z += final_az * time_delta;

        velocity_x *= damping;
        velocity_y *= damping;
        velocity_z *= damping;

        position_x += velocity_x * time_delta;
        position_y += velocity_y * time_delta;
        position_z += velocity_z * time_delta;

        output_position_x[k] = position_x;
        output_position_y[k] = position_y;
        output_position_z[k] = position_z;

        output_velocity_x[k] = velocity_x;
        output_velocity_y[k] = velocity_y;
        output_velocity_z[k] = velocity_z;

        output_position[l + offset] = (float4)(position_x, position_y, position_z, m);

    }
}


__kernel void
IntegrateSystemVectorized(
    __global float4 *output_position_x, 
    __global float4 *output_position_y, 
    __global float4 *output_position_z, 
    __global float4 *mass,
    __global float4 *output_velocity_x, 
    __global float4 *output_velocity_y, 
    __global float4 *output_velocity_z,
    __global float4 *input_position_x, 
    __global float4 *input_position_y, 
    __global float4 *input_position_z,
    __global float4 *input_velocity_x, 
    __global float4 *input_velocity_y, 
    __global float4 *input_velocity_z,
    float time_delta, 
    float damping, 
    float softening, 
    int body_count, 
    int body_count_per_group,
    __global float4 *output_position, 
    int start_index )
{
    int index = get_global_id(0);

    float softening_squared = softening * softening;
    float4 position_x, position_y, position_z, m;
    float4 current_x1, current_y1, current_z1, current_mass1;
    float4 current_x2, current_y2, current_z2, current_mass2;
    float4 velocity_x, velocity_y, velocity_z;
    float4 zero = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

    int i, j, k, l;

    int inner_loop_count = (body_count >> 2);
    int outer_loop_count = (body_count_per_group >> 2);
    int start = index * outer_loop_count + (start_index >> 2); /* because we index into a float4 */
    int offset = index * body_count_per_group + start_index;

    for (l = 0; l < outer_loop_count; l++)
    {
        k = l + start;

        position_x = input_position_x[k];
        position_y = input_position_y[k];
        position_z = input_position_z[k];
        m = mass[k];
        
        float4 final_ax = zero;
        float4 final_ay = zero;
        float4 final_az = zero;

        current_x1 = position_x.xxxx;
        current_y1 = position_y.xxxx;
        current_z1 = position_z.xxxx;
        current_mass1 = m.xxxx;

        current_x2 = position_x.yyyy;
        current_y2 = position_y.yyyy;
        current_z2 = position_z.yyyy;
        current_mass2 = m.yyyy;

        float4 acceleration_x1 = zero;
        float4 acceleration_y1 = zero;
        float4 acceleration_z1 = zero;
        
        float4 acceleration_x2 = zero;
        float4 acceleration_y2 = zero;
        float4 acceleration_z2 = zero;

        for (i = 0; i < inner_loop_count; i++)
        {

            float4 dx = input_position_x[i] - current_x1;
            float4 dy = input_position_y[i] - current_y1;
            float4 dz = input_position_z[i] - current_z1;

            float4 mi = mass[i];

            float4 distance_squared = dx * dx + dy * dy + dz * dz;
            distance_squared += softening_squared;

            float4 inverse_distance = native_rsqrt(distance_squared); 
            float4 s = (mi * inverse_distance) * (inverse_distance * inverse_distance);

            acceleration_x1 += dx * s;
            acceleration_y1 += dy * s;
            acceleration_z1 += dz * s;

            dx = input_position_x[i] - current_x2;
            dy = input_position_y[i] - current_y2;
            dz = input_position_z[i] - current_z2;

            distance_squared = dx * dx + dy * dy + dz * dz;
            distance_squared += softening_squared;

            inverse_distance = native_rsqrt(distance_squared);
            
            s = (mi * inverse_distance) * (inverse_distance * inverse_distance);

            acceleration_x2 += dx * s;
            acceleration_y2 += dy * s;
            acceleration_z2 += dz * s;
        }

        final_ax.x = acceleration_x1.x + acceleration_x1.y + acceleration_x1.z + acceleration_x1.w;
        final_ay.x = acceleration_y1.x + acceleration_y1.y + acceleration_y1.z + acceleration_y1.w;
        final_az.x = acceleration_z1.x + acceleration_z1.y + acceleration_z1.z + acceleration_z1.w;

        final_ax.y = acceleration_x2.x + acceleration_x2.y + acceleration_x2.z + acceleration_x2.w;
        final_ay.y = acceleration_y2.x + acceleration_y2.y + acceleration_y2.z + acceleration_y2.w;
        final_az.y = acceleration_z2.x + acceleration_z2.y + acceleration_z2.z + acceleration_z2.w;

        current_x1 = position_x.zzzz;
        current_y1 = position_y.zzzz;
        current_z1 = position_z.zzzz;
        current_mass1 = m.zzzz;
        
        current_x2 = position_x.wwww;
        current_y2 = position_y.wwww;
        current_z2 = position_z.wwww;
        current_mass2 = m.wwww;

        acceleration_x1 = zero;
        acceleration_y1 = zero;
        acceleration_z1 = zero;
        
        acceleration_x2 = zero;
        acceleration_y2 = zero;
        acceleration_z2 = zero;

        for (i = 0; i < inner_loop_count; i++)
        {

            float4 dx = input_position_x[i] - current_x1;
            float4 dy = input_position_y[i] - current_y1;
            float4 dz = input_position_z[i] - current_z1;

            float4 mi = mass[i];

            float4 distance_squared = dx * dx + dy * dy + dz * dz;
            distance_squared += softening_squared;

            float4 inverse_distance = native_rsqrt(distance_squared);
            float4 s = (mi * inverse_distance) * (inverse_distance * inverse_distance);

            acceleration_x1 += dx * s;
            acceleration_y1 += dy * s;
            acceleration_z1 += dz * s;

            dx = input_position_x[i] - current_x2;
            dy = input_position_y[i] - current_y2;
            dz = input_position_z[i] - current_z2;

            distance_squared = dx * dx + dy * dy + dz * dz;
            distance_squared += softening_squared;

            inverse_distance = native_rsqrt(distance_squared);
            s = (mi * inverse_distance) * (inverse_distance * inverse_distance);

            acceleration_x2 += dx * s;
            acceleration_y2 += dy * s;
            acceleration_z2 += dz * s;
        }

        final_ax.z = acceleration_x1.x + acceleration_x1.y + acceleration_x1.z + acceleration_x1.w;
        final_ay.z = acceleration_y1.x + acceleration_y1.y + acceleration_y1.z + acceleration_y1.w;
        final_az.z = acceleration_z1.x + acceleration_z1.y + acceleration_z1.z + acceleration_z1.w;

        final_ax.w = acceleration_x2.x + acceleration_x2.y + acceleration_x2.z + acceleration_x2.w;
        final_ay.w = acceleration_y2.x + acceleration_y2.y + acceleration_y2.z + acceleration_y2.w;
        final_az.w = acceleration_z2.x + acceleration_z2.y + acceleration_z2.z + acceleration_z2.w;

        velocity_x = input_velocity_x[k];
        velocity_y = input_velocity_y[k];
        velocity_z = input_velocity_z[k];

        velocity_x += final_ax * time_delta;
        velocity_y += final_ay * time_delta;
        velocity_z += final_az * time_delta;
        
        velocity_x *= damping;
        velocity_y *= damping;
        velocity_z *= damping;

        position_x += velocity_x * time_delta;
        position_y += velocity_y * time_delta;
        position_z += velocity_z * time_delta;

        output_position_x[k] = position_x;
        output_position_y[k] = position_y;
        output_position_z[k] = position_z;

        output_velocity_x[k] = velocity_x;
        output_velocity_y[k] = velocity_y;
        output_velocity_z[k] = velocity_z;

        output_position[4*l+0+offset] = (float4)(position_x.x, position_y.x, position_z.x, m.x);
        output_position[4*l+1+offset] = (float4)(position_x.y, position_y.y, position_z.y, m.y);
        output_position[4*l+2+offset] = (float4)(position_x.z, position_y.z, position_z.z, m.z);
        output_position[4*l+3+offset] = (float4)(position_x.w, position_y.w, position_z.w, m.w);

    }
}
