/**
 * Copyright (C) 2021 Tim Sarbin
 * This file is part of AbyssEngine <https://github.com/AbyssEngine>.
 *
 * AbyssEngine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AbyssEngine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AbyssEngine.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "loaderprovider.h"

const char *loader_provider_get_name(loader_provider *provider) { return provider->get_name_callback(provider); }

bool loader_provider_exists(loader_provider *provider, const char *path) { return provider->exists_callback(provider, path); }

void *loader_provider_load(loader_provider *provider, const char *path, int *file_size) { return provider->load_callback(provider, path, file_size); }

void loader_provider_destroy(loader_provider *provider) { provider->destroy_callback(provider); }
