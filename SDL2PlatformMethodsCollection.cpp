#pragma once

#include<string>
#include<cassert>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<Windows.h>
#include "SDL2Util.cpp"
#include "SDL2Components.cpp"


namespace SDL2Platform {

class SDL2PlatformMethodsCollection: public MetalSlug::PlatformSpecficMethodsCollection {
public: 
    SDL_Renderer* renderer;
    SDL_Texture* frameTexture;

public: 

    SDL2PlatformMethodsCollection() {
		if (TTF_Init() == -1)
		{
			std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
			OutputDebugStringA(Util::MessageFormater::print("SDL_ttf could not initialize! TTF_Error: ", TTF_GetError(),  '\n').c_str());
			return;
		}

    }

    MetalSlug::PlatformSpecificImage* loadImage(const std::string& filename) override {
        SDL2PlatformSpecificImage *image = new SDL2PlatformSpecificImage();

		SDL_Texture* img = IMG_LoadTexture(renderer, filename.c_str());
        assert(img != NULL);
		int w, h;
		SDL_QueryTexture(img, NULL, NULL, &w, &h); 
		SDL_Rect texr; 
		texr.x = 0; 
		texr.y = 0;
		texr.w = w;
		texr.h = h;

        image->texture = img;
        image->textureRect = texr;
        image->renderer = renderer; 

        return image;
    }

    void debugLog(const std::string& debugString) override {
        OutputDebugStringA(debugString.c_str());
    }

    void renderImage(MetalSlug::PlatformSpecificImage *image, bool flipX = false, bool flipY = false) override {
        SDL2PlatformSpecificImage *img = (SDL2PlatformSpecificImage*) image;
        SDL_RendererFlip flipFlags = SDL_FLIP_NONE;

        if (flipX)
            flipFlags = SDL_FLIP_HORIZONTAL;
        if (flipY)
            flipFlags = static_cast<SDL_RendererFlip>(flipFlags | SDL_FLIP_VERTICAL);

        SDL_RenderCopyEx(renderer, img->texture, NULL, &img->textureRect, 0, NULL, flipFlags);
    }

    void fillRectangle(MetalSlug::Rect &normRect, MetalSlug::Color &color) override {
        SDL_Rect rect;
        SDL2Util::NormalizeCoordConverter::normalizedCoordToPixelCoord(normRect.x, normRect.y, rect.x, rect.y);
        SDL2Util::NormalizeCoordConverter::normalizedSizeToPixelSize(normRect.width, normRect.height, rect.w, rect.h);
        SDL2Util::NormalizeCoordConverter::toMiddleOrigin(rect.x, rect.y, rect.w, rect.h);

        // Set render color to blue ( rect will be rendered in this color )
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

        // Render rect
        SDL_RenderFillRect(renderer, &rect);
    }

    void drawRectangle(MetalSlug::Rect &normRect, MetalSlug::Color &color) override {
        SDL_Rect rect;
        SDL2Util::NormalizeCoordConverter::normalizedCoordToPixelCoord(normRect.x, normRect.y, rect.x, rect.y);
        SDL2Util::NormalizeCoordConverter::normalizedSizeToPixelSize(normRect.width, normRect.height, rect.w, rect.h);
        SDL2Util::NormalizeCoordConverter::toMiddleOrigin(rect.x, rect.y, rect.w, rect.h);

        // Set render color to blue ( rect will be rendered in this color )
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

        SDL_RenderDrawRect(renderer, &rect);
    }

    void fillCircle(MetalSlug::Circle& circle, MetalSlug::Color &color) override {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

        int centerX;
        int centerY;
        int radius;
        SDL2Util::NormalizeCoordConverter::normalizedCoordToPixelCoord(circle.x, circle.y, centerX, centerY);
        SDL2Util::NormalizeCoordConverter::normalizedSizeToPixelSize(circle.r, circle.r, radius, radius);

        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                if (x * x + y * y <= radius * radius) {
                    SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
                }
            }
        }
    }

    void drawCircle(MetalSlug::Circle& circle, MetalSlug::Color &color) override {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        int centerX;
        int centerY;
        int radius;
        SDL2Util::NormalizeCoordConverter::normalizedCoordToPixelCoord(circle.x, circle.y, centerX, centerY);
        SDL2Util::NormalizeCoordConverter::normalizedSizeToPixelSize(circle.r, circle.r, radius, radius);

        int x = radius - 1;
        int y = 0;
        int dx = 1;
        int dy = 1;
        int err = dx - (radius << 1);

        while (x >= y) {
            SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
            SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
            SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
            SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
            SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
            SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
            SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);

            if (err <= 0) {
                y++;
                err += dy;
                dy += 2;
            }

            if (err > 0) {
                x--;
                dx += 2;
                err += dx - (radius << 1);
            }
        }
    }

    void drawPoint(MetalSlug::Point& p, MetalSlug::Color &color) override {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        int x, y;
        SDL2Util::NormalizeCoordConverter::normalizedCoordToPixelCoord(p.x, p.y, x, y);
		SDL_RenderDrawPoint(renderer, x, y);
    }

    void drawLine(MetalSlug::Point& a, MetalSlug::Point& b, MetalSlug::Color &color) override {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

        int ax, ay, bx, by;
        SDL2Util::NormalizeCoordConverter::normalizedCoordToPixelCoord(a.x, a.y, ax, ay);
        SDL2Util::NormalizeCoordConverter::normalizedCoordToPixelCoord(b.x, b.y, bx, by);

        SDL_RenderDrawLine(renderer, ax, ay, bx, by);
    }

    MetalSlug::GameText* createText(int x, int y, int fontSize) override {
        SDL2GameText* text = new SDL2GameText(renderer, x, y, fontSize);
        return text;
    }

    void drawText(MetalSlug::GameText *text) override {
        SDL2GameText* txt = (SDL2GameText*) text;
		SDL_RenderCopy(renderer, txt->message, NULL, &txt->pixelRect);
    }

    void drawCapsule(MetalSlug::Capsule& capsule, MetalSlug::Color &color) {
        // Set the drawing color
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

        int startX;
        int startY;
        int endX;
        int endY;
        int radius;
        SDL2Util::NormalizeCoordConverter::normalizedCoordToPixelCoord(capsule.start.x, capsule.start.y, startX, startY);
        SDL2Util::NormalizeCoordConverter::normalizedCoordToPixelCoord(capsule.end.x, capsule.end.y, endX, endY);
        SDL2Util::NormalizeCoordConverter::normalizedSizeToPixelSize(capsule.r, capsule.r, radius, radius);


        // Draw the cylindrical part of the capsule
        SDL_RenderDrawLine(renderer, capsule.start.x, capsule.start.y, capsule.end.x, capsule.end.y);

        // Calculate the direction vector of the capsule
        float dx = endX - startX;
        float dy = endY - startY;
        float length = std::sqrt(dx * dx + dy * dy);

        // Normalize the direction vector
        float nx = dx / length;
        float ny = dy / length;

        // Calculate the perpendicular vector
        float px = -ny;
        float py = nx;

        // Calculate the offsets for the end caps
        float offsetX = px * radius;
        float offsetY = py * radius;

        // Calculate the coordinates for the end cap points
        int cap1X1 = (int)(startX + offsetX);
        int cap1Y1 = (int)(startY + offsetY);
        int cap1X2 = (int)(startX - offsetX);
        int cap1Y2 = (int)(startY - offsetY);

        int cap2X1 = (int)(endX + offsetX);
        int cap2Y1 = (int)(endY + offsetY);
        int cap2X2 = (int)(endX - offsetX);
        int cap2Y2 = (int)(endY - offsetY);

        // Draw the end caps of the capsule
        SDL_RenderDrawLine(renderer, cap1X1, cap1Y1, cap2X1, cap2Y1);
        SDL_RenderDrawLine(renderer, cap1X2, cap1Y2, cap2X2, cap2Y2);

        MetalSlug::Circle circleStart = {capsule.start.x, capsule.start.y, capsule.r};
        MetalSlug::Circle circleEnd = {capsule.end.x, capsule.end.y, capsule.r};
        drawCircle(circleStart, color);
        drawCircle(circleEnd, color);
    }
};

}