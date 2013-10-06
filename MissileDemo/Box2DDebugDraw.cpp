/*
 * Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
 *
 * iPhone port by Simon Oliver - http://www.simonoliver.com - http://www.handcircus.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

//
// File modified for cocos2d integration
// http://www.cocos2d-iphone.org
//

#import "cocos2d.h"

using namespace cocos2d;
#include "Box2DDebugDraw.h"

Box2dDebugDraw::Box2dDebugDraw()
{
   mShaderProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_Position_uColor);
   mColorLocation = glGetUniformLocation( mShaderProgram->getProgram(), "u_color");
}

void Box2dDebugDraw::DrawPolygon(const b2Vec2* old_vertices, int32 vertexCount, const b2Color& color)
{
   mShaderProgram->use();
   mShaderProgram->setUniformsForBuiltins();
   
   ccVertex2F *vertices = new ccVertex2F[vertexCount];
   
   for( int i=0;i<vertexCount;i++)
   {
      CCPoint tmp = Viewport::Instance().Convert(old_vertices[i]);
      vertices[i].x = tmp.x;
      vertices[i].y = tmp.y;
   }
   
   mShaderProgram->setUniformLocationWith4f(mColorLocation, color.r, color.g, color.b, 1);
   glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
   glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
   
   delete []vertices;
   
   CC_INCREMENT_GL_DRAWS(1);
   
   CHECK_GL_ERROR_DEBUG();
}

void Box2dDebugDraw::DrawAABB(b2AABB* aabb, const b2Color& color)
{
   
   mShaderProgram->use();
   mShaderProgram->setUniformsForBuiltins();
   
   mShaderProgram->setUniformLocationWith4f(mColorLocation, color.r, color.g, color.b, 1);
   
   CCPoint lowerBound = Viewport::Instance().Convert(aabb->lowerBound);
   CCPoint upperBound = Viewport::Instance().Convert(aabb->upperBound);
   
   
	GLfloat	glVertices[] =
   {
		lowerBound.x , lowerBound.y ,
		upperBound.x , lowerBound.y ,
      upperBound.x , upperBound.y ,
		lowerBound.x , upperBound.y
	};
   
   glVertexAttribPointer(kCCVertexAttrib_Position, 4, GL_FLOAT, GL_FALSE, 0, glVertices);
   
   glDrawArrays(GL_LINES, 0, 2);
   
   CC_INCREMENT_GL_DRAWS(1);
   
   CHECK_GL_ERROR_DEBUG();
}

void Box2dDebugDraw::DrawSolidPolygon(const b2Vec2* old_vertices, int32 vertexCount, const b2Color& color)
{
   mShaderProgram->use();
   mShaderProgram->setUniformsForBuiltins();
   
   ccVertex2F *vertices = new ccVertex2F[vertexCount];
   
   for( int i=0;i<vertexCount;i++)
   {
      CCPoint tmp = Viewport::Instance().Convert(old_vertices[i]);
      vertices[i].x = tmp.x;
      vertices[i].y = tmp.y;
   }
   
   mShaderProgram->setUniformLocationWith4f(mColorLocation, color.r*0.5f, color.g*0.5f, color.b*0.5f, 0.75f);
   
   glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
   
   glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
   
   mShaderProgram->setUniformLocationWith4f(mColorLocation, color.r, color.g, color.b, 1);
   glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
   
   delete []vertices;
   
   CC_INCREMENT_GL_DRAWS(2);
   
   CHECK_GL_ERROR_DEBUG();
}

void Box2dDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
   mShaderProgram->use();
   mShaderProgram->setUniformsForBuiltins();
   
   const float32 k_segments = 16.0f;
   const int vertexCount=16;
   const float32 k_increment = 2.0f * b2_pi / k_segments;
   float32 theta = 0.0f;
   
   GLfloat                glVertices[vertexCount*2];
   for (int32 i = 0; i < k_segments; ++i)
   {
      CCPoint v = Viewport::Instance().Convert(center + radius * b2Vec2(cosf(theta), sinf(theta)));
      glVertices[i*2]=v.x;
      glVertices[i*2+1]=v.y;
      theta += k_increment;
   }
   
   mShaderProgram->setUniformLocationWith4f(mColorLocation, color.r, color.g, color.b, 1);
   glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, glVertices);
   
   glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
   
   CC_INCREMENT_GL_DRAWS(1);
   
   CHECK_GL_ERROR_DEBUG();
}

void Box2dDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
   mShaderProgram->use();
   mShaderProgram->setUniformsForBuiltins();
   
   const float32 k_segments = 16.0f;
   const int vertexCount=16;
   const float32 k_increment = 2.0f * b2_pi / k_segments;
   float32 theta = 0.0f;
   
   GLfloat                glVertices[vertexCount*2];
   for (int32 i = 0; i < k_segments; ++i)
   {
      CCPoint v = Viewport::Instance().Convert(center + radius * b2Vec2(cosf(theta), sinf(theta)));
      glVertices[i*2]=v.x;
      glVertices[i*2+1]=v.y;
      theta += k_increment;
   }
   
   mShaderProgram->setUniformLocationWith4f(mColorLocation, color.r*0.5f, color.g*0.5f, color.b*0.5f, 0.75f);
   glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, glVertices);
   glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
   
   mShaderProgram->setUniformLocationWith4f(mColorLocation, color.r, color.g, color.b, 1);
   glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
   
   DrawSegment(center,center+radius*axis,color);
   
   CC_INCREMENT_GL_DRAWS(2);
   
   CHECK_GL_ERROR_DEBUG();
}

void Box2dDebugDraw::DrawSegment(const b2Vec2& p1Org, const b2Vec2& p2Org, const b2Color& color)
{
   mShaderProgram->use();
   mShaderProgram->setUniformsForBuiltins();
   
   mShaderProgram->setUniformLocationWith4f(mColorLocation, color.r, color.g, color.b, 1);
   
   CCPoint p1 = Viewport::Instance().Convert(p1Org);
   CCPoint p2 = Viewport::Instance().Convert(p2Org);
   
   
   GLfloat glVertices[] =
   {
      p1.x, p1.y,
      p2.x, p2.y
   };
   
   glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, glVertices);
   
   glDrawArrays(GL_LINES, 0, 2);
   
   CC_INCREMENT_GL_DRAWS(1);
   
   CHECK_GL_ERROR_DEBUG();
}

void Box2dDebugDraw::DrawTransform(const b2Transform& xf)
{
   b2Vec2 p1 = xf.p, p2;
   const float32 k_axisScale = 0.4f;
   p2 = p1 + k_axisScale * xf.q.GetXAxis();
   DrawSegment(p1, p2, b2Color(1,0,0));
   
   p2 = p1 + k_axisScale * xf.q.GetYAxis();
   DrawSegment(p1,p2,b2Color(0,1,0));
}