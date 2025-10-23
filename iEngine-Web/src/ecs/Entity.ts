export type EntityId = number;

let nextEntityId: EntityId = 1;

export function newEntityId(): EntityId {
    return nextEntityId++;
}