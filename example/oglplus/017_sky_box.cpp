/**
 *  @example oglplus/017_sky_box.cpp
 *  @brief Shows how to render a textured sky box
 *
 *  @oglplus_screenshot{017_sky_box}
 *
 *  Copyright 2008-2011 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include <oglplus/gl.hpp>
#include <oglplus/all.hpp>

#include <oglplus/bound/texture.hpp>

#include <oglplus/images/load.hpp>

#include "example.hpp"

namespace oglplus {

class SkyBoxExample : public Example
{
private:
	Context gl;

	Program prog;

	LazyUniform<Mat4f> projection_matrix, camera_matrix;

	VertexArray sky_box;

	Buffer corners, indices;

	Texture env_map;
public:
	SkyBoxExample(void)
	 : prog()
	 , projection_matrix(prog, "ProjectionMatrix")
	 , camera_matrix(prog, "CameraMatrix")
	{
		VertexShader vs;
		vs.Source(StrLit(
			"#version 330\n"
			"uniform mat4 ProjectionMatrix, CameraMatrix;"
			"mat4 Matrix = ProjectionMatrix*CameraMatrix;"

			"in vec3 Corner;"
			"out vec3 vertTexCoord;"
			"void main(void)"
			"{"
			"	gl_Position = Matrix * vec4(Corner * 100.0, 1.0);"
			"	vertTexCoord = Corner;"
			"}"
		));
		vs.Compile();
		prog.AttachShader(vs);

		FragmentShader fs;
		fs.Source(StrLit(
			"#version 330\n"
			"uniform samplerCube EnvMap;"
			"in vec3 vertTexCoord;"
			"out vec4 fragColor;"
			"void main(void)"
			"{"
			"	fragColor = vec4(texture(EnvMap, normalize(vertTexCoord)).rgb, 1.0);"
			"}"
		));
		fs.Compile();
		prog.AttachShader(fs);

		prog.Link();
		prog.Use();

		sky_box.Bind();

		GLfloat sky_box_corners[8*3] = {
			-1.0f,-1.0f,-1.0f,
			+1.0f,-1.0f,-1.0f,
			-1.0f,+1.0f,-1.0f,
			+1.0f,+1.0f,-1.0f,
			-1.0f,-1.0f,+1.0f,
			+1.0f,-1.0f,+1.0f,
			-1.0f,+1.0f,+1.0f,
			+1.0f,+1.0f,+1.0f
		};
		corners.Bind(Buffer::Target::Array);
		Buffer::Data(Buffer::Target::Array, 8*3, sky_box_corners);
		VertexAttribArray vert_attr(prog, "Corner");
		vert_attr.Setup(3, DataType::Float);
		vert_attr.Enable();

		GLuint sky_box_indices[6*5] = {
			1, 3, 5, 7, 9,
			4, 6, 0, 2, 9,
			2, 6, 3, 7, 9,
			4, 0, 5, 1, 9,
			5, 7, 4, 6, 9,
			0, 2, 1, 3, 9
		};
		indices.Bind(Buffer::Target::ElementArray);
		Buffer::Data(Buffer::Target::ElementArray, 6*5, sky_box_indices);

		gl.Enable(Capability::PrimitiveRestart);
		gl.PrimitiveRestartIndex(9);

		{
			UniformSampler(prog, "EnvMap").Set(0);
			Texture::Active(0);

			auto bound_tex = oglplus::Bind(env_map, Texture::Target::CubeMap);
			bound_tex.MinFilter(TextureMinFilter::Linear);
			bound_tex.MagFilter(TextureMagFilter::Linear);
			bound_tex.WrapS(TextureWrap::ClampToEdge);
			bound_tex.WrapT(TextureWrap::ClampToEdge);
			bound_tex.WrapR(TextureWrap::ClampToEdge);

			Texture::Image2D(
				Texture::CubeMapFace(0),
				images::LoadTexture("cloudy_day-cm_0", false, false)
			);
			Texture::Image2D(
				Texture::CubeMapFace(1),
				images::LoadTexture("cloudy_day-cm_1", false, false)
			);
			Texture::Image2D(
				Texture::CubeMapFace(2),
				images::LoadTexture("cloudy_day-cm_2", false, false)
			);
			Texture::Image2D(
				Texture::CubeMapFace(3),
				images::LoadTexture("cloudy_day-cm_3", false, false)
			);
			Texture::Image2D(
				Texture::CubeMapFace(4),
				images::LoadTexture("cloudy_day-cm_4", false, false)
			);
			Texture::Image2D(
				Texture::CubeMapFace(5),
				images::LoadTexture("cloudy_day-cm_5", false, false)
			);
		}

		gl.ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		gl.ClearDepth(1.0f);
	}

	void Reshape(GLuint width, GLuint height)
	{
		gl.Viewport(width, height);
		projection_matrix = CamMatrixf::PerspectiveX(
			Degrees(48),
			float(width)/height,
			1, 100
		);
	}

	void Render(double time)
	{
		auto camera = CamMatrixf::Orbiting(
			Vec3f(),
			3.0,
			FullCircles(time / 13.0),
			Degrees(-SineWave(time / 19.0) * 85)
		);
		camera_matrix.Set(camera);

		gl.Clear().ColorBuffer().DepthBuffer();
		gl.DrawElements(PrimitiveType::TriangleStrip, 6*5, DataType::UnsignedInt);
	}

	bool Continue(double time)
	{
		return time < 90.0;
	}

	double ScreenshotTime(void) const
	{
		return 1.0;
	}
};

std::unique_ptr<Example> makeExample(const ExampleParams& /*params*/)
{
	return std::unique_ptr<Example>(new SkyBoxExample);
}

} // namespace oglplus
