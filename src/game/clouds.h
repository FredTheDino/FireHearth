Renderer::ParticleSystem cloud_particles_1;
Renderer::ParticleSystem cloud_particles_2;
Renderer::ParticleSystem cloud_particles_3;
Renderer::ParticleSystem cloud_particles_4;
Renderer::ParticleSystem cloud_particles_5;

f32 CLOUD_SIZE = 4;

void setCloudSys(Renderer::ParticleSystem& sys) {
	sys = Renderer::create_particle_system(20, V2(-100, 15));
	sys.one_alpha = true;
	sys.one_size = true;
	sys.alive_time = {50, 50};
	sys.rotation = {PI, PI};
	sys.velocity_dir = {};
	sys.velocity = {5, 10};
	sys.damping = {1, 1};
	sys.position_y = {-10, 20};
}

void createCloudSystems() {
	setCloudSys(cloud_particles_1);
	cloud_particles_1.add_sprite(ASSET_CLOUD_1, 0, 0, 21, 8);
	cloud_particles_1.spawn_size = {2.1f * CLOUD_SIZE, 2.1f * CLOUD_SIZE};
	cloud_particles_1.width = {2.1, 2.1};
	cloud_particles_1.height = {0.8, 0.8};

	setCloudSys(cloud_particles_2);
	cloud_particles_2.add_sprite(ASSET_CLOUD_2, 0, 0, 27, 9);
	cloud_particles_2.spawn_size = {2.7f * CLOUD_SIZE, 2.7f * CLOUD_SIZE};
	cloud_particles_2.width = {2.7, 2.7};
	cloud_particles_2.height = {0.9, 0.9};

	setCloudSys(cloud_particles_3);
	cloud_particles_3.add_sprite(ASSET_CLOUD_3, 0, 0, 20, 9);
	cloud_particles_3.spawn_size = {2.0f * CLOUD_SIZE, 2.0f * CLOUD_SIZE};
	cloud_particles_3.width = {2.0, 2.0};
	cloud_particles_3.height = {0.9, 0.9};

	setCloudSys(cloud_particles_4);
	cloud_particles_4.add_sprite(ASSET_CLOUD_4, 0, 0, 11, 5);
	cloud_particles_4.spawn_size = {1.1f * CLOUD_SIZE, 1.1f * CLOUD_SIZE};
	cloud_particles_4.width = {1.1, 1.1};
	cloud_particles_4.height = {0.5, 0.5};

	setCloudSys(cloud_particles_5);
	cloud_particles_5.add_sprite(ASSET_CLOUD_5, 0, 0, 19, 6);
	cloud_particles_5.spawn_size = {1.9f * CLOUD_SIZE, 1.9f * CLOUD_SIZE};
	cloud_particles_5.width = {1.9, 1.9};
	cloud_particles_5.height = {0.6, 0.6};
}

void spawnCloud() {
	int sys = random_int()%5 + 1;
	switch(sys){
	case 1:
		cloud_particles_1.spawn();
		break;
	case 2:
		cloud_particles_2.spawn();
		break;
	case 3:
		cloud_particles_3.spawn();
		break;
	case 4:
		cloud_particles_4.spawn();
		break;
	case 5:
		cloud_particles_5.spawn();
	}
}

void updateClouds(f32 delta) {
	cloud_particles_1.update(delta);
	cloud_particles_2.update(delta);
	cloud_particles_3.update(delta);
	cloud_particles_4.update(delta);
	cloud_particles_5.update(delta);
}

void drawClouds() {
	cloud_particles_1.draw();
	cloud_particles_2.draw();
	cloud_particles_3.draw();
	cloud_particles_4.draw();
	cloud_particles_5.draw();
}

