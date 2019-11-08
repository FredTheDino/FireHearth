struct Entity {
    // Basic constructor
    Entity(Vec2 pos, Vec2 dim, AssetID image = NO_ASSET, f32 rotation = 0) :
        pos(pos),
        dim(dim),
        image(image),
        rotation(rotation) {}

    virtual void update(f32 delta) = 0;

    Vec2 pos;
    Vec2 dim;
    AssetID image;
    f32 rotation;
};
