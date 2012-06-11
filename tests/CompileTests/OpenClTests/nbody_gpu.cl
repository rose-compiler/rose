//
// File:       nbody_gpu.cl
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

#define LOOP_UNROLL 4

float4
ComputeForce(
    float4 force, 
    float4 position_a, 
    float4 position_b, 
    float softening_squared)
{
    float4 r;
    r.x = position_a.x - position_b.x;
    r.y = position_a.y - position_b.y;
    r.z = position_a.z - position_b.z;
    r.w = 1.0f;
    
    float distance_squared = mad( r.x, r.x, mad( r.y, r.y, r.z*r.z) );
    distance_squared += softening_squared;

    float inverse_distance = native_rsqrt(distance_squared);
    float inverse_distance_cubed = inverse_distance * inverse_distance * inverse_distance;
    float s = position_a.w * inverse_distance_cubed;

    force.x += r.x * s;
    force.y += r.y * s;
    force.z += r.z * s;

    return force;
}

__kernel void
IntegrateSystem(
    __global float4 *output_position,
    __global float4 *output_velocity,
    __global float4 *input_position,
    __global float4 *input_velocity,
    const float time_delta,
    const float damping,
    const float softening,
    const int body_count,
    const int start_index,
    const int end_index,
    __local float4 *shared_position)
{
    int index = get_global_id(0);
    int local_id = get_local_id(0);
    int tile_size = get_local_size(0);

    int tile = 0;
    index += start_index;

    float4 position = input_position[index]; 
    float softening_squared = softening * softening;

    float4 force = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

    int i, j;
    for (i = 0; i < body_count; i += tile_size, tile++)
    {
        size_t local_index = (tile * tile_size + local_id);
        float4 local_position = input_position[local_index];
        shared_position[local_id] = local_position;

        barrier(CLK_LOCAL_MEM_FENCE);

        for (j = 0; j < tile_size; )
        {
            force = ComputeForce(force, shared_position[j++], position, softening_squared);

#if LOOP_UNROLL >= 1
            force = ComputeForce(force, shared_position[j++], position, softening_squared);
#endif

#if LOOP_UNROLL >= 2
            force = ComputeForce(force, shared_position[j++], position, softening_squared);
            force = ComputeForce(force, shared_position[j++], position, softening_squared);
#endif

#if LOOP_UNROLL >= 4
            force = ComputeForce(force, shared_position[j++], position, softening_squared);
            force = ComputeForce(force, shared_position[j++], position, softening_squared);
            force = ComputeForce(force, shared_position[j++], position, softening_squared);
            force = ComputeForce(force, shared_position[j++], position, softening_squared);
#endif
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    float4 velocity = input_velocity[index];

    velocity.x += force.x * time_delta;
    velocity.y += force.y * time_delta;
    velocity.z += force.z * time_delta;
    velocity.x *= damping;
    velocity.y *= damping;
    velocity.z *= damping;
    position.x += velocity.x * time_delta;
    position.y += velocity.y * time_delta;
    position.z += velocity.z * time_delta;

    output_position[index] = position;
    output_velocity[index] = velocity;
}
